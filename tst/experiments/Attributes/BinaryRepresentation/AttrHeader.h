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

#ifndef _Attr_Header_h_
#define _Attr_Header_h_

#include <stdint.h>

#include "Utils.h"

/*!
 * Attribute-related convenience struct for ByteCount.
 */
template <typename Attr>
struct ValueByteCount {
	static const uint16_t value = ByteCount<typename Attr::value_type, Attr::value>::value;
};
/*!
 * Attribute-related convenience struct for BitCount.
 */
template <typename Attr>
struct ValueBitCount {
	static const uint16_t value = BitCount<typename Attr::value_type, Attr::value>::value;
};

union AttributeElementHeader {
	enum {
        nonSystemCategory = 0xF
	};

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
	private:
		// The category starts from the 5th bit
		static const uint8_t catOffset    = 4;
		// The extension bit is always the 4th bit
		static const uint8_t extBitOffset = 3;
		// For system attributes the VOL (value-or-length) bit is the 3rd
		static const uint8_t volBitOffset = 2;

		// System
		//       CAT                       EXT                     VOL                     LEN
		static const uint8_t case1 = (Attr::id << catOffset) | (0x0 << extBitOffset) | (0x1 << volBitOffset) | (0x00); // Value fits unextended
		static const uint8_t case2 = (Attr::id << catOffset) | (0x1 << extBitOffset) | (0x1 << volBitOffset) | (0x00); // Value fits extended
		static const uint8_t case3 = (Attr::id << catOffset) | (0x0 << extBitOffset) | (0x0 << volBitOffset) | (ValueByteCount<Attr>::value & 0x3);        // Length fits unextended
		static const uint8_t case4 = (Attr::id << catOffset) | (0x1 << extBitOffset) | (0x0 << volBitOffset) | ((ValueByteCount<Attr>::value >> 8) & 0x3); // Length fits extended

		// Non-system
		//       CAT                  EXT                     LEN
		static const uint8_t case5 = (0xF << catOffset) | (0x0 << extBitOffset) | (ValueByteCount<Attr>::value & 0x7);        // Length fits unextended
		static const uint8_t case6 = (0xF << catOffset) | (0x1 << extBitOffset) | ((ValueByteCount<Attr>::value >> 8) & 0x7); // Length fits extended

	public:
		// value will be on of the 6 cases above depending on the given Attribute
		static const uint8_t value = CaseSelector<Attr, uint8_t, case1, case2, case3, case4, case5, case6>::value;
};

template <typename Attr>
struct AttributeHeaderWriter {
	private:
		// The byte count for the attribute header (It could also include a part of the attribute's
		//  value if the VOL switch is set)
		static const uint8_t size = CaseSelector<Attr, uint8_t, 1, 1, 1, 2, (1 + 1), (1 + 1 + 1)>::value;

		static const bool res2 = (ValueBitCount<Attr>::value < 9) ? false : true;

		static const bool useOr = CaseSelector<Attr, bool, true, res2, false, false, false, false>::value;

		enum {
			writeLowerLengthBits     = 0x10,
			writeType                = 0x11,
			writeLowerLenBitsAndType = 0x12
		};

		// We use this to decide in the code below if further bytes must be manipulated
		static const uint8_t toWrite = CaseSelector<Attr, uint8_t, 0, 0, 0, writeLowerLengthBits, writeType, writeLowerLenBitsAndType>::value;

		uint8_t data[size];

	public:
		AttributeHeaderWriter() {
			// Either use an OR operation (to not overwrite existing bits already set
			//  by writing the value) or directly assign the header byte since there is
			//  no danger overwriting
			if (useOr) {
				data[0] |= (FirstByteOfHeader<Attr>::value);
			} else {
				data[0] = (FirstByteOfHeader<Attr>::value);
			}

			printCase<Attr>();

			// Handle eventually further written bytes (depending on extension, VOL etc.)
			if (toWrite == writeLowerLengthBits) {
				data[1] = (ValueByteCount<Attr>::value & 0xFF);
			} else if (toWrite == writeType) {
				data[1] = (Attr::id & 0xFF);
			} else if (toWrite == writeLowerLenBitsAndType) {
				data[1] = (ValueByteCount<Attr>::value & 0xFF);
				data[2] = (Attr::id & 0xFF);
			}
		}
};

#endif
