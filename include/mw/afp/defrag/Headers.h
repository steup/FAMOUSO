/*******************************************************************************
 *
 * Copyright (c) 2009-2010 Philipp Werner <philipp.werner@st.ovgu.de>
 *                         Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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


#ifndef __HEADERS_H_FCE771847176E1__
#define __HEADERS_H_FCE771847176E1__


#include "debug.h"


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {


                /*!
                 * \brief AFP Headers of a received fragment
                 */
                template <class DCP>
                class Headers {

                        typedef typename DCP::SizeProp::elen_t   elen_t;
                        typedef typename DCP::SizeProp::flen_t   flen_t;
                        typedef typename DCP::SizeProp::fcount_t fcount_t;

                        typedef class DCP::DemuxPolicy::EventSeqHeaderPolicy EventSeqHeader;
                        typedef class DCP::EventDataReconstructionPolicy::FECHeaderPolicy FECHeader;

                    protected:

                        /// Length of all headers, zero encodes that an error occured while reading headers
                        flen_t all_header_length;

                        /// Length of the extension headers
                        flen_t ext_header_length;

                    public:

                        /// True if current fragment is the first fragment of an event
                        bool first_fragment;

                        /// Fragment sequence number
                        fcount_t fseq;

                        /// Extension header: event sequence number
                        EventSeqHeader eseq;

                        /// Extension header: forward error correction
                        FECHeader fec;


                        /*!
                         * \brief Create Headers object and read headers from data.
                         *
                         * Check for errors via error() afterwards.
                         */
                        Headers(const uint8_t * data) {
                            // TODO: security: check fragment_length (additional parameter before reading from data)
                            all_header_length = 0;                  // Default: error status
                            ext_header_length = 0;

                            // Read basic header
                            uint8_t h = data[0];

                            flen_t b_header_length = 1;             // basic header length
                            while (h & 0x80) {
                                b_header_length++;
                                h <<= 1;
                            }

                            bool one_more_header = h & 0x40;        // extension header?
                            first_fragment = h & 0x20;

                            if (b_header_length > sizeof(fseq) || b_header_length > 6) {
                                ::logging::log::emit< ::logging::Warning>()
                                    << PROGMEMSTRING("AFP: Receiving to large event! Dropping fragment!")
                                    << ::logging::log::endl;
                                fseq = 0;
                                return;
                            }

                            // Read fragment sequence number
                            fseq = data[0] & (0x3f >> b_header_length);
                            for (uint8_t i = 1; i < b_header_length; i++) {
                                fseq <<= 8;
                                fseq |= data[i];
                            }

                            data += b_header_length;

                            // Read extension headers
                            flen_t e_header_length;
                            while (one_more_header) {
                                if (eseq.check(data))
                                    e_header_length = eseq.read_header(data);
                                else if (fec.check(data))
                                    e_header_length = fec.read_header(data);
                                else {
                                    // Header not supported
                                    ::logging::log::emit< ::logging::Error>()
                                        << PROGMEMSTRING("AFP: Unknown or unsupported extension header ")
                                        << (*data & 0x1f)
                                        << PROGMEMSTRING("! Dropping fragment!")
                                        << ::logging::log::endl;
                                    return;
                                }
                                if (!e_header_length) {
                                    // Error reading header
                                    return;
                                }
                                one_more_header = *data & 0x40;
                                ext_header_length += e_header_length;
                                data += e_header_length;
                            }

                            // Set value removing error status
                            all_header_length = b_header_length + ext_header_length;
                        }

                        /// Returns true if an error occured while reading headers
                        bool error() const {
                            return all_header_length == 0;
                        }

                        /// Returns length of all headers (basic + extension headers)
                        flen_t length() const {
                            return all_header_length;
                        }

                        /// Returns length of extension headers
                        flen_t ext_length() const {
                            return ext_header_length;
                        }
                };


            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __HEADERS_H_FCE771847176E1__

