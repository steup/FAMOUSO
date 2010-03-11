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

#include "boost/mpl/list.hpp"
#include "boost/mpl/begin.hpp"
#include "boost/mpl/end.hpp"
#include "boost/mpl/next.hpp"
#include "boost/mpl/deref.hpp"
#include "boost/mpl/size.hpp"

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
struct ByteCountCalculator {
	enum {
		value = BitCountToByteCount<BitCountCalculator<Attr::value>::value>::value
	};
};

/*!
 * Template struct allowing to decide the 6 different cases for the
 *  structure of an attribute header:
 *
 * 1. System attribute, value fits unextended
 * 2. System attribute, value fits extended
 * 3. System attribute, length fits unextended
 * 4. System attribute, length fits extended
 *
 * 5. Non-system attribute, length fits unextended
 * 6. Non-system attribute, length fits extended
 *
 * The specific case is decided on the given attribute type
 *  and depending on the decided case one of the arguments
 *  res1 to res6 is calculated
 */
template <typename Attr, uint8_t res1, uint8_t res2, uint8_t res3, uint8_t res4, uint8_t res5, uint8_t res6>
struct CaseSelector {
private:
	enum {
		// The number of bits used by the attribute's value
		bitCount = BitCountCalculator<Attr::value>::value,

		// The whole bytes used by the attribute's value
		byteCount = ByteCountCalculator<Attr>::value
	};

public:
	enum {
		value = (Attr::isSystem) ?
					( // For system attributes
					(bitCount < 3) ?
						// If the value fits the header byte unextended, this is the first case
						res1 :

						// Check if the value can be written directly (without a length)
						(bitCount < 11) ?
								// If the value fits 10 bits (2 bits of the header byte and 8 bits of the extension)
								//  this is the second case
								res2 :

								// If the length fits the header byte unextended, we would write the header byte + "length-many" bytes
								//  (Length-values up to 3 fit into two bits, which would be remaining)
								(byteCount < 4) ?
									// So this is the third case
									res3 :
									// Otherwise we would extend the header byte and so need one more byte
									//  which is the fourth case
									res4
					) :

					( // For non system attributes
					// The last part of the header byte is always the length, we now must find out, if the
					//  header must be extended for the length
					(byteCount < 8) ?
						// The length fits unextended, so we need 1 byte for the header itself, one byte
						//  for the type and "length-many" bytes for the value so this is the fifth case
						res5 :
						// We must extend the header byte, that is one more byte compared to the first
						//  case is needed which is the sixth and last case
						res6
					)
	};
};

/*!
 * Calculates the overall number of bytes used by the binary representation
 *  of the given attribute, that is the header and the value.
 *
 * /tparam Attr The attribute type of which the overall size should be
 *  calculated
 */
template <typename Attr>
struct SizeCalculator {
private:
	enum {
		byteCount = ByteCountCalculator<Attr>::value
	};

public:
	enum {
		value = CaseSelector<Attr, 1, 2, (1 + byteCount), (2 + byteCount), (1 + 1 + byteCount), (2 + 1 + byteCount)>::value
	};
};

template <typename Attr>
void writeValue(uint8_t* const array) {
	const uint64_t bigEndian = htonll(Attr::value);
	const uint8_t* ptr       = (uint8_t*) &bigEndian;

	for (int i = 8 - ByteCountCalculator<Attr>::value, j = 0; i < 8; ++i, ++j) {
		array[j] = ptr[i];
	}
}

template <typename ValueType, ValueType Value, uint8_t ID, bool IsSystem = false>
struct ExtendedAttribute {
	typedef ExtendedAttribute type;

    static const ValueType value = Value;

    // TODO: To also support other types of values (of which the bitcount etc. cannot
    //  be determined by the provided means) it would be useful to store bitcount etc.
    //  as an enum value (This one could be "overridden" by inheriting classes to
    //  provide their special bitcounts etc.)
	enum {
		id       = ID,
		isSystem = IsSystem,
	};

	// The data array contains the value of this attribute (with the least byte count needed)
	uint8_t data[ByteCountCalculator<type>::value];

	ExtendedAttribute() {
		// Initialize the member array "data" to the binary representation
		//  of this attribute's value
		// The byte offset is given by invocation of a placement new from the outside,
		//  so it's just up to the attribute class to perform the correct alignment of
		//  the binary value

		writeValue<type>(data);
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

/*!
 * Represents the first byte of the header of the given attribute. As
 *  the attribute header in general consists of up to 3 bytes this
 *  struct is not sufficient on its own.
 *
 * /tparam Attr The attribute to calculate the first header byte for
 */
template <typename Attr>
struct FirstByteOfHeader {
	enum {
		catOffset    = 4,
		extBitOffset = 3,
		volBitOffset = 2,

		// System
		//       CAT                       EXT                     VOL                     LEN
		case1 = (Attr::id << catOffset) | (0x0 << extBitOffset) | (0x1 << volBitOffset) | (0x00), // Value fits unextended
		case2 = (Attr::id << catOffset) | (0x1 << extBitOffset) | (0x1 << volBitOffset) | (0x00), // Value fits extended
		case3 = (Attr::id << catOffset) | (0x0 << extBitOffset) | (0x0 << volBitOffset) | (ByteCountCalculator<Attr>::value & 0x3),        // Length fits unextended
		case4 = (Attr::id << catOffset) | (0x1 << extBitOffset) | (0x0 << volBitOffset) | ((ByteCountCalculator<Attr>::value >> 8) & 0x3), // Length fits extended

		// Non-system
		//       CAT                  EXT                     LEN
		case5 = (0xF << catOffset) | (0x0 << extBitOffset) | (ByteCountCalculator<Attr>::value & 0x5),        // Length fits unextended
		case6 = (0xF << catOffset) | (0x1 << extBitOffset) | ((ByteCountCalculator<Attr>::value >> 8) & 0x5), // Length fits extended

		value = CaseSelector<Attr, case1, case2, case3, case4, case5, case6>::value
	};
};

template <typename Attr>
struct AttributeHeaderWriter {
	enum {
		// The byte count for the attribute header (It could also include a part of the attribute's
		//  value if the VOL switch is set)
		size = CaseSelector<Attr, 1, 1, 1, 2, (1 + 1), (1 + 1 + 1)>::value,

		res2  = (BitCountCalculator<Attr::value>::value < 9) ? false : true,
		useOr = CaseSelector<Attr, true, res2, false, false, false, false>::value,

		writeLowerLengthBits     = 0x10,
		writeType                = 0x11,
		writeLowerLenBitsAndType = 0x12,

		// We use this to decide in the code below if further bytes must be manipulated
		toWrite = CaseSelector<Attr, 0, 0, 0, writeLowerLengthBits, writeType, writeLowerLenBitsAndType>::value
	};

	uint8_t data[size];

	AttributeHeaderWriter() __attribute__((noinline)) {
		if (useOr) {
			data[0] |= (FirstByteOfHeader<Attr>::value & 0xFF);
		} else {
			data[0] = (FirstByteOfHeader<Attr>::value & 0xFF);
		}

		switch (toWrite) {
			case writeLowerLengthBits: {
				data[1] = (ByteCountCalculator<Attr>::value & 0xFF);
				break;
			}

			case writeType: {
				data[1] = (Attr::id & 0xFF);
				break;
			}

			// TODO: This case could be linked with the first one (write the lower 8
			//  length bits to data[1]) by fall-through
			case writeLowerLenBitsAndType: {
				data[1] = (ByteCountCalculator<Attr>::value & 0xFF);
				data[2] = (Attr::id & 0xFF);
				break;
			}
		}
	}
};

template <typename Attr>
struct ValueOffsetCalculator {
	enum {
		// For the second case (value fits extended) it is necessary to check if
		//  the value only needs one byte, which would mean that it is simply written
		//  into the second byte
		res2 = (BitCountCalculator<Attr::value>::value < 9) ? 1 : 0,

		value = CaseSelector<Attr, 0, res2, 1, 2, 2, 3>::value
	};
};

template <typename AttrType>
void testAttribute(const char* text = "") {
	::logging::log::emit() << text << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::hex;
	::logging::log::emit() << "Testing attribute with ID " << (int) AttrType::id << " and value " << (uint64_t) AttrType::value << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::dec;
	::logging::log::emit() << ::logging::log::endl;

	uint8_t data[SizeCalculator<AttrType>::value];

	::logging::log::emit() << "Value offset: " << (uint64_t) ValueOffsetCalculator<AttrType>::value << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::endl;

	// Construct the attribute into the data array at the correct offset
	new (&data[ValueOffsetCalculator<AttrType>::value]) AttrType;

	::logging::log::emit() << "Attribute size     : " << (uint64_t) SizeCalculator<AttrType>::value << ::logging::log::endl;
	::logging::log::emit() << "Attribute bit count: " << (uint64_t) BitCountCalculator<AttrType::value>::value << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::endl;

	// Now construct the header into the array
	new (&data[0]) AttributeHeaderWriter<AttrType>;

	::logging::log::emit() << "Header size: " << (int) AttributeHeaderWriter<AttrType>::size << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::endl;

	print(data, sizeof(data));

	::logging::log::emit() << "------------------------------------------" << ::logging::log::endl;
	::logging::log::emit() << ::logging::log::endl;
}

template <uint16_t listCount>
struct ListHeaderWriter {
	enum {
		extension = (listCount > 0x7F),
		size      = (extension ? 2 : 1)
	};

	uint8_t data[size];

	ListHeaderWriter() {
		::logging::log::emit() << "Writing " << listCount << ::logging::log::endl;

		if (extension) {
			data[0] = 0x80 | ((listCount >> 8) & 0x7F);
			data[1] = (listCount & 0xFF);
		} else {
			data[0] = (listCount & 0x7F);
		}
	}
};

template <typename A, typename B>
struct is_same {
	enum {
		value = false
	};
};
template <typename A>
struct is_same<A, A> {
	enum {
		value = true
	};
};

template <typename AttrSeq, typename Iter = typename boost::mpl::begin<AttrSeq>::type>
struct AttribSequence {
private:
	typedef boost::mpl::deref<Iter> curAttr;
	typedef boost::mpl::next<Iter>  iterNext;

	typedef ListHeaderWriter<boost::mpl::size<AttrSeq>::type::value> listHeader;
	//typedef ListHeaderWriter<0x7F> listHeader;
	//typedef ListHeaderWriter<0xFF> listHeader;

public:
	typedef AttribSequence type;

	enum {
		isFirst     = is_same<Iter, typename boost::mpl::begin<AttrSeq>::type>::value,

		offset      = listHeader::size,

		overallSize = (isFirst ? listHeader::size : 0) +
				      SizeCalculator<typename curAttr::type>::value +
				      AttribSequence<AttrSeq, typename iterNext::type>::overallSize
	};

	uint8_t data[overallSize];

	AttribSequence() {
		if (isFirst) {
			new (&data[0]) listHeader;
		}

		// Construct the current attribute's value
		new (&data[offset + ValueOffsetCalculator<typename curAttr::type>::value]) typename curAttr::type;
		// Construct the current attribute's header
		new (&data[offset]) AttributeHeaderWriter<typename curAttr::type>;

		// Let the rest of the sequence construct its attributes into the rest of the array
		new (&data[offset + SizeCalculator<typename curAttr::type>::value]) AttribSequence<AttrSeq, typename iterNext::type>;
	}
};

template <typename AttrSeq>
struct AttribSequence<AttrSeq, typename boost::mpl::end<AttrSeq>::type> {
	enum {
		overallSize = 0
	};
};

int main() {
	typedef ExtendedAttribute<uint8_t,  0x3,     1, true>::type   a1;
	typedef ExtendedAttribute<uint8_t,  0x8,     2, true>::type   a2;
	typedef ExtendedAttribute<uint16_t, 0x3FF,   3, true>::type   a3;
	typedef ExtendedAttribute<uint16_t, 0x4FF,   4, true>::type   a4;
	typedef ExtendedAttribute<uint64_t, 0x1FFFFFF, 5, true>::type a5;

	typedef ExtendedAttribute<uint8_t, 2, 6, false>  ::type                   a6;
	typedef ExtendedAttribute<uint64_t, 0x1FFFFFFFFFFFFFFull, 7, false>::type a7;

	typedef boost::mpl::list<a1, a2, a3, a4, a5, a6, a7>::type attribList;

	AttribSequence<attribList> sequence;

	print(sequence.data, sizeof(sequence.data));

	// return (0);

	// Test cases:

	// I. System attributes

	// 1. Value fits unextended
	testAttribute<a1>("val unex");
	// 2. Value fits extended (but has less than 9 bits itself)
	testAttribute<a2>("val ex1");
	// 3. Value fits extended (but has at least 9 bits itself)
	testAttribute<a3>("val ex2");
	// 4. Length fits unextended
	testAttribute<a4>("len unex");
	// 5. Length fits extended
	testAttribute<a5>("len ex");

	// II. Non-system attributes

	// 1. Length fits unextended
	testAttribute<a6>();
	// 2. Length fits extended
	testAttribute<a7>();

}


