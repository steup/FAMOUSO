/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Marcus Foerster <MarcusFoerster1@gmx.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id:
 *
 ******************************************************************************/

#ifndef _ByteCount_h_
#define _ByteCount_h_

#include <stdint.h>

#include "boost/mpl/if.hpp"
#include "boost/mpl/int.hpp"
#include "boost/mpl/plus.hpp"

template <typename ValueType, ValueType Value>
struct BitCount {
	static const uint16_t value = (1 + BitCount<ValueType, (Value >> 1)>::value);
};
template <>
struct BitCount<uint8_t, 0> {
	static const uint16_t value = 0;
};
template <>
struct BitCount<uint16_t, 0> {
	static const uint16_t value = 0;
};
template <>
struct BitCount<uint32_t, 0> {
	static const uint16_t value = 0;
};
template <>
struct BitCount<uint64_t, 0> {
	static const uint16_t value = 0;
};
template <int8_t Value>
struct BitCount<int8_t, Value> {
	static const uint16_t value = (Value < 0) ? (sizeof(int8_t) * 8) : BitCount<uint8_t, static_cast<uint8_t>(Value)>::value;
};
template <int16_t Value>
struct BitCount<int16_t, Value> {
	static const uint16_t value = (Value < 0) ? (sizeof(int16_t) * 8) : BitCount<uint16_t, static_cast<uint16_t>(Value)>::value;
};
template <int32_t Value>
struct BitCount<int32_t, Value> {
	static const uint16_t value = (Value < 0) ? (sizeof(int32_t) * 8) : BitCount<uint32_t, static_cast<uint32_t>(Value)>::value;
};
template <int64_t Value>
struct BitCount<int64_t, Value> {
	static const uint16_t value = (Value < 0) ? (sizeof(int64_t) * 8) : BitCount<uint64_t, static_cast<uint64_t>(Value)>::value;
};

/*!
 * Calculates the number of whole bytes needed to store the given
 *  value.
 */
template <typename ValueType, ValueType Value>
struct ByteCount {
	private:
		static const uint16_t bitCount = BitCount<ValueType, Value>::value;

	public:
		static const uint16_t value = (bitCount % 8 == 0) ? (bitCount / 8) : ((bitCount / 8) + 1);
};


// Runtime bit / byte count calculation

template <typename ValueType>
const uint16_t getBitCount(ValueType value);

template <>
const uint16_t getBitCount<uint64_t>(uint64_t value) {
	uint16_t result = 0;

	while (value > 0) {
		++result;

		value >>= 1;
	}

	return (result);
}
template <>
const uint16_t getBitCount<uint8_t>(uint8_t value) {
	return (getBitCount(static_cast<uint64_t>(value)));
}
template <>
const uint16_t getBitCount<uint16_t>(uint16_t value) {
	return (getBitCount(static_cast<uint64_t>(value)));
}
template <>
const uint16_t getBitCount<uint32_t>(uint32_t value) {
	return (getBitCount(static_cast<uint64_t>(value)));
}

template <>
const uint16_t getBitCount<int8_t>(int8_t value) {
	return ((value < 0) ? sizeof(int8_t) : getBitCount(static_cast<uint64_t>(value)));
}
template <>
const uint16_t getBitCount<int16_t>(int16_t value) {
	return ((value < 0) ? sizeof(int16_t) : getBitCount(static_cast<uint64_t>(value)));
}
template <>
const uint16_t getBitCount<int32_t>(int32_t value) {
	return ((value < 0) ? sizeof(int32_t) : getBitCount(static_cast<uint64_t>(value)));
}
template <>
const uint16_t getBitCount<int64_t>(int64_t value) {
	return ((value < 0) ? sizeof(int64_t) : getBitCount(static_cast<uint64_t>(value)));
}

const uint16_t bitCountToByteCount(const uint16_t value) {
	// TODO: Implement this using a lookup map for the limit values, e.g.
	//  (value < 0x100) -> byteCount = 1 etc.

	if (value % 8 == 0) {
		return (value / 8);
	} else {
		return ((value / 8) + 1);
	}
}

#endif
