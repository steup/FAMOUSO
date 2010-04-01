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

#ifndef _Utils_h_
#define _Utils_h_

#include <stdint.h>

#include "ByteCount.h"

#include "logging/logging.h"

#define TO_HEX ::logging::log::emit() << ::logging::log::hex
#define TO_DEC ::logging::log::emit() << ::logging::log::dec
#define DBG_MSG(x) ::logging::log::emit() << x << ::logging::log::endl

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
template <typename Attr, typename ResultType, ResultType res1, ResultType res2, ResultType res3, ResultType res4, ResultType res5, ResultType res6>
struct CaseSelector {
	private:
		// The number of bits used by the attribute's value
		static const uint16_t bitCount = BitCount<typename Attr::value_type, Attr::value>::value;
		// The whole bytes used by the attribute's value
		static const uint16_t byteCount = ByteCount<typename Attr::value_type, Attr::value>::value;

	public:
		static const ResultType value = (Attr::isSystem) ?
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
																			);
};

template <typename T1, typename T2>
struct is_same {
		static const bool value = false;
};
template <typename T1>
struct is_same<T1, T1> {
		static const bool value = true;
};

void print(const uint8_t* array, uint8_t length) {
	::logging::log::emit() << ::logging::log::hex;

	for (uint8_t i = 0; i < length; ++i) {
		::logging::log::emit() << '{' << reinterpret_cast<size_t>(&array[i]) << "}=";
		::logging::log::emit() << '[' << static_cast<uint16_t>(array[i]) << ']';
	}

	::logging::log::emit() << ::logging::log::endl;

	// ::logging::log::emit() << ::logging::log::dec;
}

template <typename Attr>
void printCase() {
	::logging::log::emit() << "Selected case " << CaseSelector<Attr, uint16_t, 1, 2, 3, 4, 5, 6>::value << ::logging::log::endl;
}

#endif
