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

void print(uint8_t* array, uint8_t length) {
	::logging::log::emit() << ::logging::log::hex;

	for (uint8_t i = 0; i < length; ++i) {
		::logging::log::emit() << '[' << (int) array[i] << ']';
	}

	::logging::log::emit() << ::logging::log::endl;

	::logging::log::emit() << ::logging::log::dec;
}

/*!
 * Calculates the number of bits needed to represent the given
 *  unsigned value.
 *
 * /tparam Value The value to calculate the bit count for
 */
template <uint64_t Value>
struct BitCountCalculator {
	enum {
		value = (Value < 0x2) ? 1 : (1 + BitCountCalculator<(Value >> 1)>::value)
	};
};
template <>
struct BitCountCalculator<0> {
	enum {
		value = 0
	};
};

template <uint64_t BitCount>
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

						// Check if the value can be written directly (without a length)
						(Attr::bitCount < 11) ?
								// If the value fits 10 bits (2 bits of the header byte and 8 bits of the extension)
								//  only 2 bytes are needed
								2:

								// If the length fits the header byte unextended, we would write the header byte + "length-many" bytes
								//  (Length-values up to 3 fit into two bits, which would be remaining)
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

template <bool isSystem, uint8_t bitCount>
void writeValue(uint8_t* const array, const uint64_t value) {
	if ((isSystem) &&  (bitCount > 2) && (bitCount < 9)) {
		// Special case for system attributes where the value fits extended but
		//  itself fits one byte (that ís less than 9 bits)
		array[1] = (value & 0xFF);

		::logging::log::emit() << "Writing at the first way: ";
		print(array, 2);
	} else {
		const uint64_t bigEndian = htonll(value);
		const uint8_t* ptr       = (uint8_t*) &bigEndian;

		for (int i = 8 - BitCountToByteCount<bitCount>::value, j = 0; i < 8; ++i, ++j) {
			array[j] = ptr[i];
		}

		::logging::log::emit() << "Writing at the second way: ";
		print(array, BitCountToByteCount<bitCount>::value);
	}
}

template <typename ValueType, ValueType Value, uint16_t ID, bool IsSystem = false>
struct ExtendedAttribute : public famouso::mw::attributes::EmptyAttribute {
	typedef ExtendedAttribute type;

    static const ValueType value=Value;

	enum {
		id       = ID,
		isSystem = IsSystem,
		bitCount = BitCountCalculator<Value>::value,
		size     = SizeCalculator<type>::value
	};

	// The data array contains the right aligned value of this attribute
	uint8_t data[BitCountToByteCount<bitCount>::value];


	ExtendedAttribute() {
		// Initialize the member array "data" to the binary representation
		//  of this attribute's value
		// The byte offset is given by invocation of a placement new from the outside,
		//  so it's just up to the attribute class to perform the correct alignment of
		//  the binary value

		writeValue<isSystem, bitCount>(data, Value);
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
private:
	enum {
		byteCount = BitCountToByteCount<Attr::bitCount>::value
	};

public:
	enum {
		// The byte count for the attribute header (It could also include a part of the attribute's
		//  value if the VOL switch is set)

		size = (Attr::isSystem) ?
				( // For system attributes
				(Attr::bitCount < 3) ?
					// If the value fits the header byte unextended, this is the only byte needed
					1 :

					// Check if the value can be written directly (without a length)
					(Attr::bitCount < 11) ?
							// If the value fits 10 bits (2 bits of the header byte and 8 bits of the extension)
							//  only 2 bytes are needed, while one of them is the header byte
							1:

							// If the length fits the header byte unextended, we would write the header byte + "length-many" bytes
							//  (Length-values up to 3 fit into two bits, which would be remaining)
							(byteCount < 4) ?
								1 :
								// Otherwise we would extend the header byte and so need one more byte
								2
				) :

				( // For non system attributes
				// The last part of the header byte is always the length, we now must find out, if the
				//  header must extended for the length
				(byteCount < 8) ?
					// The length fits unextended, so we need 1 byte for the header itself, one byte
					//  for the type and "length-many" bytes for the value
					(1 + 1) :
					// We must extend the header byte, that is one more byte compared to the first
					//  case is needed
					(1 + 1 + 1)
				)
	};

	uint8_t data[size];

	AttributeHeaderWriter() __attribute__((noinline)) {
//		AttributeElementHeader* header = (AttributeElementHeader*) data;

		if (Attr::isSystem) {
			// For system attributes the ID is always included in the header byte, so no further
			//  byte will be used for this, so the size calculated above in general tells us whether
			//  the header must be extended
			// An exception to this is that the value of a system attribute fits extended
			//  then the header is only 1 byte in size but the extension bit is set though
            data[0]=10;
//			header->extension = (size > 1) || ((Attr::bitCount > 2) && (Attr::bitCount < 11));
//
//			// System attributes write their id directly into the header byte
//			header->category = Attr::id & 0xF;
//
//			// The VOL flag is set depending on the bits used for the attribute's value
//			header->valueOrLengthSwitch = (Attr::bitCount < 11);
//
//			// If the length follows, we have some more work to do (the value would be written
//			//  by the attribute itself)
//			if (Attr::bitCount > 10) {
//				// Check if the length can be written unextended (lengths less than 4
//				//  directly fit the rest of the header byte)
//				if (BitCountToByteCount<Attr::bitCount>::value < 4) {
//					header->valueOrLength = BitCountToByteCount<Attr::bitCount>::value & 0x3;
//				} else {
//					// In the other case only the highest 2 bits of the length will be written
//					//  to the header byte directly (the lower 8 bits will be written to the
//					//  next byte)
//					header->valueOrLength = (BitCountToByteCount<Attr::bitCount>::value >> 8) & 0x3;
//
//					assert(size == 2);
//
//					// The lower 8 bits of the length
//					data[1] = (BitCountToByteCount<Attr::bitCount>::value & 0xFF);
//				}
//			}
		} else {
			// For non-system attributes which always save their ID as one byte additionally,
			//  the extension bit is set if the length must be extended, which is the case if
			//  the size calculated above is greater than 2
//			header->extension = (size > 2);
//
//			header->category = 0xF;
//
//			// For non-system attributes we have to check if the length fits into the
//			//  three remaining bits (lengths up to 7 can be written to three bits)
//			if (BitCountToByteCount<Attr::bitCount>::value < 8) {
//				header->length = (BitCountToByteCount<Attr::bitCount>::value & 0x7);
//
//				assert(size == 2);
//
//				data[1] = Attr::id;
//			} else {
//				header->length = ((BitCountToByteCount<Attr::bitCount>::value >> 8) & 0x7);
//
//				assert(size == 3);
//
//				data[1] = BitCountToByteCount<Attr::bitCount>::value & 0xFF;
//				data[2] = Attr::id;
//			}
		}
	}
};

template <typename Attr>
struct ValueOffsetCalculator {
private:
	enum {
		byteCount = BitCountToByteCount<Attr::bitCount>::value
	};

public:
	enum {
		value = (Attr::isSystem) ?
					( // For system attributes
					(Attr::bitCount < 3) ?
						// If the value fits the header byte unextended, this is the only byte needed and so
						//  the value must be written into the first byte
						0 :

						// Check if the value can be written directly (without a length)
						(Attr::bitCount < 11) ?
								// If the value fits 10 bits (2 bits of the header byte and 8 bits of the extension)
								//  only 2 bytes are needed, while one of them is the header byte, the value is written
								//  into the first byte, too, in this case
								0:

								// If the length fits the header byte unextended, we would write the header byte + "length-many" bytes
								//  (Length-values up to 3 fit into two bits, which would be remaining)
								(byteCount < 4) ?
									1 :
									// Otherwise we would extend the header byte and so need one more byte
									2
					) :

					( // For non system attributes
					// The last part of the header byte is always the length, we now must find out, if the
					//  header must extended for the length
					(byteCount < 8) ?
						// The length fits unextended, so we need 1 byte for the header itself, one byte
						//  for the type and "length-many" bytes for the value
						2 :
						// We must extend the header byte, that is one more byte compared to the first
						//  case is needed
						3
					)
	};
};

template <typename AttrType>
void testAttribute() {
	::logging::log::emit() << ::logging::log::hex;
	::logging::log::emit() << "Testing attribute with ID " << (int) AttrType::id << " and value " << (int) AttrType::value << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::dec;
	::logging::log::emit() << ::logging::log::endl;

	// Init data to zeros
	uint8_t data[AttrType::size];
	for (int i = 0; i < AttrType::size; ++i) {
		data[i] = 0x00;
	}

	::logging::log::emit() << "Value offset: " << (int) ValueOffsetCalculator<AttrType>::value << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::endl;

	// Construct the attribute into the data array at the correct offset
	new (&data[ValueOffsetCalculator<AttrType>::value]) AttrType;
	AttrType* attr = (AttrType*) &data[ValueOffsetCalculator<AttrType>::value];
	print(attr->data, sizeof(attr->data));

	::logging::log::emit() << "Attribute size     : " << (int) AttrType::size << ::logging::log::endl;
	::logging::log::emit() << "Attribute bit count: " << (int) AttrType::bitCount << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::endl;

	// Just to see the plain header data (without the value previously written to)
	AttributeHeaderWriter<AttrType> plain;
	print(plain.data, sizeof(plain.data));

	// Now construct the same header again to the compound array
	new (&data[0]) AttributeHeaderWriter<AttrType>;

	::logging::log::emit() << "Header size: " << (int) AttributeHeaderWriter<AttrType>::size << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::endl;

	print(data, sizeof(data));

	::logging::log::emit() << "------------------------------------------" << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::endl;
}

template <uint8_t T[]>
struct Test {

};

int main() {
	// Test cases:

	// I. System attributes

	// 1. Value fits header unextended
	testAttribute<ExtendedAttribute<uint8_t, 2, 1, true> >();
	// 2. Value fits extended (but has less than 9 bits itself)
	testAttribute<ExtendedAttribute<uint8_t, 8, 2, true> >();
	// 3. Value fits extended (but has at least 9 bits itself)
	testAttribute<ExtendedAttribute<uint16_t, 0x3FF, 3, true> >();
	// 4. Length fits unextended
	testAttribute<ExtendedAttribute<uint16_t, 0x4FF, 4, true> >();
	testAttribute<ExtendedAttribute<uint64_t, 0xFFFFFFFFFFFFFFFFull, 4, true> >();

	// 5. Length fits unextended (TODO)
	// testAttribute<ExtendedAttribute<uint16_t, 0x4FF, 4, true> >();

typedef ExtendedAttribute<uint16_t, 0x3FF, 3, true> t;
BitCountCalculator<t::value>::value;
	// II. Non-system attributes

	// 1. Length fits header unextended
	testAttribute<ExtendedAttribute<uint8_t, 2, 5, false> >();

	// 2. Length fits header extended (TODO)
	// testAttribute<ExtendedAttribute<uint8_t, 2, 0xFFFFFFFF, false> >();

}
















