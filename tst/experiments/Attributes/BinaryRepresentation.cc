/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
 * $Id: template.cc 1510 2010-02-02 07:18:58Z mschulze $
 *
 ******************************************************************************/

#include "stdint.h"
#include <iostream>

#include "mw/attributes/EmptyAttribute.h"
#include "mw/attributes/TTL.h"

#define ATTRIBUTE_LIST_LENGTH_BITS 7
#define NON_EXTENSION_LENGTH_LIMIT (1 << ATTRIBUTE_LIST_LENGTH_BITS)

// Category constant for non-system attributes (system attributes will
//  have a category range of (0x0 - 0xE)
#define NON_SYSTEM_CATEGORY 0xF

// If a system attribute is not extended this is the maximum value which
//  can be written (length or value with two bits set)
#define SYS_ATTRIB_NON_EXT_LIMIT 0x3

// If a non-system attribute is not extended this is the maximum length
//  which can be written directly into the header byte
#define NON_SYS_ATTRIB_NON_EXT_LIMIT 0x7

using namespace std;
using namespace famouso::mw::attributes;

// TTL system attribute with "isSystem"-flag
template <uint8_t ttl>
class ExtTTL : public TTL<ttl> {
	public:
		enum {
			id       = NON_SYSTEM_CATEGORY - 1,
			isSystem = true,
			size     = 1 // The size should only represent the bytes used by the value
		};
};

// Test non-system attribute
template <uint64_t timestamp>
class TimeStamp : public EmptyAttribute  {
	public:
		typedef TimeStamp<0>             base_type;
		typedef tags::integral_const_tag compare_tag;
		typedef TimeStamp                type;

		enum {
			isSystem = false,
			size     = 8,
			id       = 0x0001
		};

		static uint64_t value() {
			return (timestamp);
		}

		TimeStamp() : val(timestamp) {}

		TimeStamp(const uint64_t& val) : val(val) {}

		uint64_t get() const {
			return (val);
		}

	private:
		uint64_t val;
};

/*
 *
 */
struct AttributeListHeader {
	// Lower 7 bits
	uint8_t length    : ATTRIBUTE_LIST_LENGTH_BITS;

	// Highest bit
	bool    extension : 1;
} __attribute__((packed));

/*
 *
 */
/*
struct AttributeElementHeader {
		union {
			// For system attributes
			struct {
				uint8_t valueOrLength       : 2;
				bool    valueOrLengthSwitch : 1;
			} __attribute__((packed));

			// For non system attributes
			uint8_t length : 3;
		};

		bool    extension : 1;

		// Highest 4 bits
		uint8_t category  : 4;
} __attribute__((packed));
*/

/*
union AttributeElementHeader {
    // For system attributes
    struct {
        uint8_t valueOrLength       : 2;
        uint8_t valueOrLengthSwitch : 1;
    } __attribute__((packed));

    // For non system attributes
    uint8_t length : 3;

    struct {
        uint8_t :3;
        uint8_t extension : 1;
        uint8_t category  : 4;
    } __attribute__((packed));
};
*/

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
 * \brief Writes the given attribute list size to the given
 *  byte array and returns the next available position to
 *  write into the array
 *
 * If the list size does not exceed 127 only one byte will
 *  written with the extension bit not set. Otherwise the
 *  given length will be split into two bytes. Note that
 *  lengths above 32767 are generally not supported and will
 *  silently be cut down to this limit.
 *
 * \param[in] listSize The number of attributes represented by
 *  this list header
 * \param[in,out] target The byte array to write the header into, since
 *  this is provided referenced the next available write position
 *  can be obtained after thuis method returns
 */
void writeAttributeListHeader(const uint16_t& listSize, uint8_t*& target) {
	// Directly interpret the first byte as the header struct and increment the target pointer
	AttributeListHeader* header = (AttributeListHeader*) target++;

	// FIXME: Turn the order of writing high and low byte around, so that the high byte
	//  is written first

	// The usage of bit field structs automatically implies the bit-op with a
	//  7-bit-mask here
	header->length    = listSize;
	// The extension bit must be set if the given size is not less than the
	//  limit (the limit is the first extended value)
	header->extension = !(listSize < NON_EXTENSION_LENGTH_LIMIT);

	if (header->extension) {
		// Write the length extension and afterwards increment the
		//  pointer
		*target++ = (listSize >> ATTRIBUTE_LIST_LENGTH_BITS);
	}
}

/*!
 * \brief Reads an attribute list header, i.e. its size, from the
 *  given byte array incrementing the array pointer to the next
 *  available read position.
 *
 * \param[in,out] source The byte array to read the list header from
 *
 * \return The attribute list size
 */
const uint16_t readAttributeListHeader(const uint8_t*& source) {
	const AttributeListHeader* header = (const AttributeListHeader*) source;

	uint16_t result = header->length;

	++source;

	if (header->extension) {
		result |= (*source++ << ATTRIBUTE_LIST_LENGTH_BITS);
	}

	return (result);
}

// TODO: What about byte order? At the moment the value is written as it
//  stored in memory since it is iterated as a byte array (perhaps hton and
//  ntoh would solve this)
template <typename T>
void writeValue(const T& val, const uint16_t& size, uint8_t*& target) {
	uint8_t* value = (uint8_t*) &val;

	for (int i = 0; i < size; ++i) {
		*target++ = value[i];
	}
}

template <typename T>
void readValue(T& val, const uint16_t& size, uint8_t*& source) {
	uint8_t* value = (uint8_t*) &val;

	for (int i = 0; i < size; ++i) {
		value[i] = *source++;
	}
}

// An attribute is assumed to provide the following:
//
// CATEGORY:
// - if the attribute is a system attribute by A::isSystem
//
// ID:
// - if it is a system attribute, it should provide a four bit ID by A::id
//    in the range 0-15
// - otherwise A::id() may provide an eight bit ID
//
// LENGTH:
// - the number of bytes which the attribute's value needs for representation by A::size
//
// VALUE:
// - an arbitrary length value by attribute.get()
//
template <typename A>
void writeAttribute(const A& attribute, uint8_t*& target) {
	// Interpret the first byte as a header struct and increment the pointer
	AttributeElementHeader* header = (AttributeElementHeader*) target++;

	if (A::isSystem) {
		// TODO assert (A::id != 15)

		header->category = A::id;

		// TODO: Is it assumable that an attribute value is always integral? If not, how
		//  can we check whether the value fits unextended?
		// The size flag only tells how much BYTES are used not BITS, so we cannot use
		//  this to decide the extension flag

		if (attribute.get() > SYS_ATTRIB_NON_EXT_LIMIT) {
			header->valueOrLengthSwitch = false; // In any case we write the length

			// Check if the length can be written unextended
			if (A::size > SYS_ATTRIB_NON_EXT_LIMIT) {
				// The length must be split to the extension byte
				header->extension     = true;
				header->valueOrLength = (A::size >> 8); // Write the highest two bits of the length

				// Write the lower byte of the length to the extension byte
				*target++ = A::size & 0xFF;
			} else {
				// The length can be written unextended
				header->extension     = false;
				header->valueOrLength = A::size;
			}

			// Write the attribute value to the bytes following the attribute header (and
			//  possibly the extension byte)
			writeValue(attribute.get(), A::size, target);
		} else {
			// The plain value fits the two bits
			header->extension           = false;
			header->valueOrLengthSwitch = true; // We write the value
			header->valueOrLength       = attribute.get();
		}
	} else {
		header->category = NON_SYSTEM_CATEGORY;

		// Check whether the length fits unextended
		if (A::size > NON_SYS_ATTRIB_NON_EXT_LIMIT) {
			header->extension = true;
			header->length    = (A::size >> 8); // Write the highest three bits of the length

			// Write the lower byte of the length to the extension byte
			*target++ = A::size & 0xFF;
		} else {
			header->extension = false;
			header->length    = A::size;
		}

		// In any case the is written now
		*target++ = A::id;

		// And finally the attribute value follows
		writeValue(attribute.get(), A::size, target);
	}

	// TODO Debug
	cout << "Attr       : " << typeid(attribute).name() << endl;

	cout << "ID         : " << A::id << endl;
	cout << "Size       : " << A::size << endl;
	cout << "IsSystem   : " << A::isSystem << endl;
	cout << "Value:     : " << (long) attribute.get() << endl;

	cout << endl;

	cout << "Category   : " << (int) header->category << endl;
	cout << "Extension  : " << (int) header->extension << endl;

	if (A::isSystem) {
		cout << "VOL Switch : " << (int) header->valueOrLengthSwitch << endl;
		cout << "VOL Value  : " << (int) header->valueOrLength << endl;
	} else {
		cout << "Length     : " << (int) header->length << endl;
	}

	cout << "-------------" << endl;
}

// ----------------------------------------------------

int main() {
	// Attribute list length
	const int cnt = 0x3;

	// Declared attributes
	ExtTTL<1> ttl;

	TimeStamp<0x87654321> ts1;
	TimeStamp<0x12345678> ts2;

	// The buffer to write the data into
	uint8_t  buffer[256];
	// A movable pointer to the buffer (needed by the encoding method)
	uint8_t* bufPtr = buffer;

	// Configure stream to write hex representation
	cout << hex;

	// Write the list length
	writeAttributeListHeader(cnt, bufPtr);

	cout << "Wrote " << (int) (bufPtr - buffer) << " bytes for the length" << endl << endl;

	// Write the attributes
	writeAttribute(ttl, bufPtr);
	writeAttribute(ts1, bufPtr);
	writeAttribute(ts2, bufPtr);

	// Another movable pointer to the buffer for printing the written bytes
	uint8_t* outPtr = buffer;

	// Print the bytes
	while (outPtr != bufPtr) {
		cout << "[" << (int) *outPtr++ << "]";
	}

	cout << endl;
}
