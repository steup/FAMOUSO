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
 * $Id$
 *
 ******************************************************************************/

#ifndef _AttribTests_
#define _AttribTests_

#include <stdint.h>

#include "logging/logging.h"

#include "AttrHeader.h"
#include "EmptyAttribute.h"
#include "AttribSequence.h"

template <typename AttrType>
void testAttribute(const char* text = "") {
	::logging::log::emit() << text << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::hex;
	::logging::log::emit() << "Testing attribute with ID " << (int) AttrType::id << " and value " << AttrType::value << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::dec;
	::logging::log::emit() << ::logging::log::endl;

	uint8_t data[SizeCalculator<AttrType>::value];

	::logging::log::emit() << "Value offset: " << ValueOffsetCalculator<AttrType>::value << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::endl;

	// Construct the attribute into the data array at the correct offset
	new (&data[ValueOffsetCalculator<AttrType>::value]) AttrType;

	::logging::log::emit() << "Attribute size     : " << SizeCalculator<AttrType>::value << ::logging::log::endl;
	::logging::log::emit() << "Attribute bit count: " << ValueBitCount<AttrType>::value << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::endl;

	// Now construct the header into the array
	new (&data[0]) AttributeHeaderWriter<AttrType>;

	::logging::log::emit() << "Header size: " << (int) AttributeHeaderWriter<AttrType>::size << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::endl;

	print(data, sizeof(data));

	::logging::log::emit() << "------------------------------------------" << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::endl;
}

template <uint8_t ttl>
struct TTL : public ExtendedAttribute<uint8_t, ttl, 0, true> { };

template <uint16_t delay>
struct Delay : public ExtendedAttribute<uint16_t, delay, 1, true> { };

template <uint16_t omission>
struct OmissionDegree : public ExtendedAttribute<uint16_t, omission, 2, true> { };

typedef TTL<0>   Local;
typedef TTL<1>   Body;
typedef TTL<255> World;

typedef OmissionDegree<0> None;

typedef ExtendedAttribute<uint8_t,  0x3,       1, true>::type a1;
typedef ExtendedAttribute<uint8_t,  0x8,       2, true>::type a2;
typedef ExtendedAttribute<uint16_t, 0x3FF,     3, true>::type a3;
typedef ExtendedAttribute<uint16_t, 0x4FF,     4, true>::type a4;
typedef ExtendedAttribute<uint64_t, 0x1FFFFFF, 5, true>::type a5;

typedef ExtendedAttribute<uint8_t, 2, 6, false>::type                     a6;
typedef ExtendedAttribute<uint64_t, 0x1FFFFFFFFFFFFFFull, 7, false>::type a7;

typedef ExtendedAttribute<int16_t, -20, 8, false>::type a8;

#endif
