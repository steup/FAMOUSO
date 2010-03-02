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

#include <stdint.h>

#include "util/endianness.h"
#include "mw/common/Event.h"
#include "mw/attributes/EmptyAttribute.h"

#include "debug.h"

/*!
 * Calculates the number of bits needed to represent the given
 *  unsigned value.
 *
 * /tparam Value The value to calculate the bit count for
 */
template <uint32_t Value>
struct BitCountCalculator {
	enum {
		value = (Value < 0x2) ? 1 : (1 + BitCountCalculator<Value >> 1>::value)
	};
};
template <>
struct BitCountCalculator<0> {
	enum {
		value = 0
	};
};

template <uint32_t BitCount>
struct BitCountToByteCount {
	enum {
		value = (BitCount % 8 == 0) ? (BitCount / 8) : ((BitCount / 8) + 1)
	};
};

template <typename Attr>
struct SizeCalculator {
private:
	enum {
		byteCount = BitCountToByteCount<Attr::bitCount>::value
	};

public:
	enum {
		value = (Attr::isSystem) ?
					( // For system attributes
					(Attr::bitCount < 3) ?
						// If the value fits the header byte unextended, this is the only byte needed
						1 :

						// If the length fits the header byte unextended, we would write the header byte + "length-many" bytes
						//  (Length-values up to 3 fit into two bits, which would be remaining)
						// TODO: Consider extended values
						(byteCount < 4) ?
							(1 + byteCount) :
							// Otherwise we would extend the header byte and so need one more byte
							(2 + byteCount)
					) :

					( // For non system attributes
					// The last part of the header byte is always the length, we now must find out, if the
					//  header must extended for the length
					(byteCount < 8) ?
						// The length fits unextended, so we need 1 byte for the header itself, one byte
						//  for the type and "length-many" bytes for the value
						(1 + 1 + byteCount) :
						// We must extend the header byte, that is one more byte compared to the first
						//  case is needed
						(1 + 1 + 1 + byteCount)
					)
	};
};

template <typename ValueType>
ValueType inner_hton(const ValueType& value);

template <>
uint8_t inner_hton<uint8_t>(const uint8_t& value) {
	return (value);
}
template <>
uint16_t inner_hton<uint16_t>(const uint16_t& value) {
	return (htons(value));
}
template <>
uint32_t inner_hton<uint32_t>(const uint32_t& value) {
	return (htonl(value));
}

template <typename ValueType, ValueType Value, bool IsSystem = false>
struct ExtendedAttribute : public famouso::mw::attributes::EmptyAttribute {
	typedef ExtendedAttribute<ValueType, Value, IsSystem> thisType;

	enum {
		id       = 0x12,
		isSystem = IsSystem,
		bitCount = BitCountCalculator<Value>::value,
		size     = SizeCalculator<thisType>::value
	};

	// The data array contains the right aligned value of this attribute
	uint8_t data[BitCountToByteCount<bitCount>::value];

	static const ValueType value() {
		return (Value);
	}

	ExtendedAttribute() {
		// Initialize the member array "data" to the binary representation
		//  of this attribute's value
		*((ValueType*) &data[0]) = inner_hton(Value << (8 * (sizeof(ValueType) - BitCountToByteCount<bitCount>::value)));
	}
};

union AttributeElementHeader {
    // For system attributes
    struct {
        uint8_t valueOrLength       : 2;
        uint8_t valueOrLengthSwitch : 1;
        uint8_t extension           : 1;
        uint8_t category            : 4;
    } __attribute__((packed));

    // For non system attributes
    uint8_t length : 3;
};

template <typename Attr>
struct AttributeHeaderWriter {
	enum {
		// The byte count for the attribute header
		// If the attribute's size is 1 (which includes the header) the header size is
		//  also 1, otherwise: TODO: Consider extended values
		size = (Attr::size == 1) ? 1 : Attr::size - BitCountToByteCount<Attr::bitCount>::value
	};

	uint8_t data[size];

	AttributeHeaderWriter() {
		AttributeElementHeader* header = (AttributeElementHeader*) data;

		// The extension bit will be set if the header's size is greater than 1 byte
		//  (Since this is the intuitive meaning of the extension bit)
		header->extension = (size > 1);

		if (Attr::isSystem) {
			// System attributes write their id directly into the header byte
			header->category = Attr::id & 0xF;

			// The VOL flag is set depending on the bits used for the attribute's value
			// FIXME: That is not right, if the value would take 10 bits, we also could set the
			//  VOL flag, we would have to extend but not have to write the length explicitly
			header->valueOrLengthSwitch = (Attr::bitCount < 3);

			// If the length follows, we have some more work to do (the value would be written
			//  by the attribute itself)
			if (Attr::bitCount > 2) {
				// Check if the length can be written unextended (lengths less than 4
				//  directly fit the rest of the header byte)
				if (BitCountToByteCount<Attr::bitCount>::value < 4) {
					header->valueOrLength = BitCountToByteCount<Attr::bitCount>::value & 0x3;
				} else {
					// In the other case only the highest 2 bits of the length will be written
					//  to the header byte directly (the lower 8 bits will be written to the
					//  next byte)
					header->valueOrLength = (BitCountToByteCount<Attr::bitCount>::value >> 8) & 0x3;
					// The lower 8 bits of the length
					data[1] = (BitCountToByteCount<Attr::bitCount>::value & 0xFF);
				}
			}
		} else {
			header->category = 0xF;

			// For non-system attributes we have to check if the length fits into the
			//  three remaining bits (lengths up to 7 can be written to three bits)
			if (BitCountToByteCount<Attr::bitCount>::value < 8) {
				header->length = (BitCountToByteCount<Attr::bitCount>::value & 0x7);

				data[1] = Attr::id;
			} else {
				header->length = ((BitCountToByteCount<Attr::bitCount>::value >> 8) & 0x7);

				data[1] = BitCountToByteCount<Attr::bitCount>::value & 0xFF;
				data[2] = Attr::id;
			}
		}
	}
};

template <typename Attr>
struct ValueOffsetCalculator {
	enum {
		value = (Attr::isSystem) ?
					( // For system attributes
					// First we have to check if the header size is 1 byte
					(AttributeHeaderWriter<Attr>::size == 1) ?
						(// If it is, this could mean that either the value directly fits the
						 //  header or the length directly fits the header
						(Attr::bitCount < 3) ?
							// The value directly fits the header, so the offset is 0
							0 :
							// The length directly fits the header, so the offset is 1
							1
						) :
						( // If it's not this could either mean that the value is written
						  //  extended or the length is written extended
							1 // TODO
						)
					) :
					( // For non-system attributes
					2 // TODO
					)
	};
};


void print(uint8_t* array, uint8_t length) {
	::logging::log::emit() << ::logging::log::hex;

	for (uint8_t i = 0; i < length; ++i) {
		::logging::log::emit() << '[' << (int) array[i] << ']';
	}

	::logging::log::emit() << ::logging::log::endl;
}

int main() {
	typedef ExtendedAttribute<uint32_t, 0x8FFFF> AttrType;

	uint8_t data[AttrType::size];

	// Init data to zeros
	for (int i = 0; i < AttrType::size; ++i) {
		data[i] = 0x00;
	}

	new (&data[ValueOffsetCalculator<AttrType>::value]) AttrType;

	AttrType* attr = (AttrType*) &data[ValueOffsetCalculator<AttrType>::value];

	::logging::log::emit() << "Attribute size: " << (int) AttrType::size << ::logging::log::endl;

	print(attr->data, sizeof(attr->data));

	AttributeHeaderWriter<AttrType> w;

	print(w.data, sizeof(w.data));

	print(data, sizeof(data));
}
