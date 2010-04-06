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

#ifndef __AVR__
#include "mw/afp/shared/hexdump.h"
#endif


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {


                /*!
                 * \brief AFP Headers of a received fragment
                 */
                template <class AFPDC>
                class Headers {

                        typedef typename AFPDC::SizeProp::elen_t   elen_t;
                        typedef typename AFPDC::SizeProp::flen_t   flen_t;
                        typedef typename AFPDC::SizeProp::fcount_t fcount_t;

                        typedef class AFPDC::DemuxPolicy::EventSeqHeaderPolicy EventSeqHeader;
                        typedef class AFPDC::EventDataReconstructionPolicy::FECHeaderPolicy FECHeader;

                    protected:

                        /// Length of all headers
                        flen_t all_header_length;

                        /// True if an error occurs while reading headers
                        bool header_error;

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
                            header_error = false;

                            // Read basic header
                            flen_t header_length = 1;
                            uint8_t h = data[0];
                            while (h & 0x80) {
                                header_length++;
                                h <<= 1;
                            }

                            bool one_more_header = h & 0x40;
                            first_fragment = h & 0x20;

                            if (header_length > sizeof(fseq)) {
                                ::logging::log::emit< ::logging::Warning>() << "AFP: Receiving to large event! Dropping fragment!" << ::logging::log::endl;
                                header_error = true;
                                return;
                            }

                            // Read fragment sequence number
                            fseq = data[0] & (0x3f >> header_length);
                            for (uint8_t i = 1; i < header_length; i++) {
                                fseq <<= 8;
                                fseq |= data[i];
                            }

                            all_header_length = header_length;
                            data += header_length;

                            // Read extension headers
                            while (one_more_header) {
                                if (eseq.check(data))
                                    header_length = eseq.read_header(data);
                                else if (fec.check(data))
                                    header_length = fec.read_header(data);
                                else {
                                    // Header not supported
                                    ::logging::log::emit< ::logging::Error>() << "AFP: Unknown or unsupported extension header "
                                                                              << (*data & 0x2f) << "! Dropping fragment!"
                                                                              << ::logging::log::endl;
#ifndef __AVR__
                                    afp::shared::hexdump(data, 8);
#endif
                                    header_error = true;
                                    return;
                                }
                                if (!header_length) {
                                    // Error reading header
                                    header_error = true;
                                }
                                one_more_header = *data & 0x40;
                                all_header_length += header_length;
                                data += header_length;
                            }
                        }

                        /// Returns true if an error occured while reading headers
                        bool error() const {
                            return header_error;
                        }

                        /// Returns length of headers
                        flen_t length() const {
                            return all_header_length;
                        }
                };


            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __HEADERS_H_FCE771847176E1__

