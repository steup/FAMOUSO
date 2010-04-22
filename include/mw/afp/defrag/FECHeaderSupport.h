/*******************************************************************************
 *
 * Copyright (c) 2009-2010 Philipp Werner <philipp.werner@st.ovgu.de>
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


#ifndef __FECHEADERSUPPORT_H_62EF7504454E4B__
#define __FECHEADERSUPPORT_H_62EF7504454E4B__


#include "util/endianness.h"


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {


                /*!
                 * \brief Policy: Support forward error correction extension header
                 *
                 * This class is automatically selected as FEC header policy if
                 * FECEventReconstructor is used as event reconstruction policy.
                 *
                 * Alternative policies: NoFECHeaderSupport
                 */
                template <class DCP>
                class FECHeaderSupport {

                        typedef typename DCP::SizeProp::flen_t   flen_t;
                        typedef typename DCP::SizeProp::fcount_t fcount_t;

                        /// FEC header structure
                        struct __attribute__((packed)) Header1001 {
                            uint8_t head;           ///< Bits: e01? 1001 - e: one more extension header; ?: don't care
                            uint8_t red;            ///< Redundancy
                            uint32_t k;             ///< Count of data packets
                            uint16_t len_rest;      ///< Length of the last (non-redundancy) fragment
                        };

                        uint8_t red;            ///< Redundancy
                        fcount_t k;             ///< Count of data packets
                        flen_t len_rest;        ///< length = k * MTU - len_rest

                    public:

                        /// Constructor
                        FECHeaderSupport()
                                : k(0) {
                        }

                        /*!
                         * \brief Return whether this header can be found at the beginning
                         *        of data.
                         */
                        bool check(const uint8_t * data) {
                            return (*data & 0xbf) == 0x29;
                        }

                        /*!
                         * \brief Read header from the beginning of data.
                         *
                         * \returns Length of the header. Zero on error.
                         */
                        flen_t read_header(const uint8_t * data) {
                            const Header1001 * h = reinterpret_cast<const Header1001 *>(data);

                            red = h->red;

                            // If fcount_t or flen_t are to small we may cut
                            // away information... check this.
                            bool error = false;

                            uint32_t k32 = ntohl(h->k);
                            k = k32;
                            if (k != k32)
                                error = true;

                            uint16_t len_rest16 = ntohs(h->len_rest);
                            len_rest = len_rest16;
                            if (len_rest != len_rest16)
                                error = true;

                            return error ? 0 : sizeof(Header1001);
                        }

                        /*!
                         * \brief Returns whether this header was read.
                         */
                        bool occurs() const {
                            return k != 0;
                        }

                        /*!
                         * \brief Returns the redundancy read from the header.
                         *
                         * Should be only called if occured() returns true.
                         */
                        uint32_t get_red() const {
                            return red;
                        }

                        /*!
                         * \brief Returns the number of non-redundancy fragments read from the header.
                         *
                         * Should be only called if occured() returns true.
                         */
                        uint32_t get_k() const {
                            return k;
                        }

                        /*!
                         * \brief Returns of the last non-redundancy fragment's length.
                         *
                         * Should be only called if occured() returns true.
                         */
                        uint32_t get_len_rest() const {
                            return len_rest;
                        }
                };


            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __FECHEADERSUPPORT_H_62EF7504454E4B__

