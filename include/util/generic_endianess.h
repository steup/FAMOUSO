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

        /*!
         * \brief Conversion from host to network byte order
         *
         * \param value The value which should be converted
         *
         * \return The converted value
         */
        static inline const uint8_t hton(const uint8_t value) {
            return (value);
        }

        /*!
         * \copydoc famouso::util::hton
         */
        static inline const int8_t hton(const int8_t value) {
            return (value);
        }

        /*!
         * \copydoc famouso::util::hton
         */
        static inline const uint16_t hton(const uint16_t value) {
            return (htons(value));
        }

        /*!
         * \copydoc famouso::util::hton
         */
        static inline const int16_t hton(const int16_t value) {
            return (htons(value));
        }

        /*!
         * \copydoc famouso::util::hton
         */
        static inline const uint32_t hton(const uint32_t value) {
            return (htonl(value));
        }

        /*!
         * \copydoc famouso::util::hton
         */
        static inline const int32_t hton(const int32_t value) {
            return (htonl(value));
        }

        /*!
         * \copydoc famouso::util::hton
         */
        static inline const uint64_t hton(const uint64_t value) {
            return (htonll(value));
        }

        /*!
         * \copydoc famouso::util::hton
         */
        static inline const int64_t hton(const int64_t value) {
            return (htonll(value));
        }

        /*!
         * \brief Conversion from network to host byte order
         *
         * \param value The value which should be converted
         *
         * \return The converted value
         */
        static inline const uint8_t ntoh(const uint8_t value) {
            return (value);
        }

        /*!
         * \copydoc famouso::util::ntoh
         */
        static inline const int8_t ntoh(const int8_t value) {
            return (value);
        }

        /*!
         * \copydoc famouso::util::ntoh
         */
        static inline const uint16_t ntoh(const uint16_t value) {
            return (ntohs(value));
        }

        /*!
         * \copydoc famouso::util::ntoh
         */
        static inline const int16_t ntoh(const int16_t value) {
            return (ntohs(value));
        }

        /*!
         * \copydoc famouso::util::ntoh
         */
        static inline const uint32_t ntoh(const uint32_t value) {
            return (ntohl(value));
        }

        /*!
         * \copydoc famouso::util::ntoh
         */
        static inline const int32_t ntoh(const int32_t value) {
            return (ntohl(value));
        }

        /*!
         * \copydoc famouso::util::ntoh
         */
        static inline const uint64_t ntoh(const uint64_t value) {
            return (ntohll(value));
        }

        /*!
         * \copydoc famouso::util::ntoh
         */
        static inline const int64_t ntoh(const int64_t value) {
            return (ntohll(value));
        }

    } // end namespace util
} // end namespace famouso

#endif // _generic_endianess_
