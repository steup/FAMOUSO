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

#ifndef _generic_endianess_
#define _generic_endianess_

#include <stdint.h>

#include "util/endianness.h"

namespace famouso {
	namespace util {

		// Generic host to network conversion

		/*!
		 * \brief Conversion from host to network byte order for generic
		 *  data types
		 *
		 * \tparam ValType The data type of which a value should be
		 *  converted
		 *
		 * \param value The value which should be converted
		 *
		 * \return The converted value
		 */
		template <typename ValType>
		inline const ValType hton(const ValType& value);

		template <> inline const uint8_t hton<uint8_t>(const uint8_t& value) {
			return (value);
		}
		template <> inline const int8_t hton<int8_t>(const int8_t& value) {
			return (value);
		}
		template <> inline const uint16_t hton<uint16_t>(const uint16_t& value) {
			return (htons(value));
		}
		template <> inline const int16_t hton<int16_t>(const int16_t& value) {
			return (hton(reinterpret_cast<const uint16_t&>(value)));
		}
		template <> inline const uint32_t hton<uint32_t>(const uint32_t& value) {
			return (htonl(value));
		}
		template <> inline const int32_t hton<int32_t>(const int32_t& value) {
			return (hton(reinterpret_cast<const uint32_t&>(value)));
		}
		template <> inline const uint64_t hton<uint64_t>(const uint64_t& value) {
			return (htonll(value));
		}
		template <> inline const int64_t hton<int64_t>(const int64_t& value) {
			return (hton(reinterpret_cast<const uint64_t&>(value)));
		}

		// Generic network to host conversion

		/*!
		 * \brief Conversion from network to host byte order for generic
		 *  data types
		 *
		 * \tparam ValType The data type of which a value should be
		 *  converted
		 *
		 * \param value The value which should be converted
		 *
		 * \return The converted value
		 */
		template <typename ValType>
		inline const ValType ntoh(const ValType& value);

		template <> inline const uint8_t ntoh<uint8_t>(const uint8_t& value) {
			return (value);
		}
		template <> inline const int8_t ntoh<int8_t>(const int8_t& value) {
			return (value);
		}
		template <> inline const uint16_t ntoh<uint16_t>(const uint16_t& value) {
			return (ntohs(value));
		}
		template <> inline const int16_t ntoh<int16_t>(const int16_t& value) {
			return (ntoh(reinterpret_cast<const uint16_t&>(value)));
		}
		template <> inline const uint32_t ntoh<uint32_t>(const uint32_t& value) {
			return (ntohl(value));
		}
		template <> inline const int32_t ntoh<int32_t>(const int32_t& value) {
			return (ntoh(reinterpret_cast<const uint32_t&>(value)));
		}
		template <> inline const uint64_t ntoh<uint64_t>(const uint64_t& value) {
			return (ntohll(value));
		}
		template <> inline const int64_t ntoh<int64_t>(const int64_t& value) {
			return (ntoh(reinterpret_cast<const uint64_t&>(value)));
		}

	} // end namespace util
} // end namespace famouso

#endif // _generic_endianess_
