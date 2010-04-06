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


#ifndef __USE32BITEVENTSEQ_H_06F00D78E8653E__
#define __USE32BITEVENTSEQ_H_06F00D78E8653E__


#include "util/endianness.h"


namespace famouso {
    namespace mw {
        namespace afp {
            namespace frag {


                /*!
                 * \brief Policy: Include 32 bit event seqence number in each fragment
                 *
                 * Using this event sequence number policy is necessary if package loss or/and
                 * reordering can happen and/or duplicates are possible in the network.
                 *
                 * Alternative policies: UseNoEventSeq
                 */
                template <class AFPC>
                class Use32BitEventSeq {

                        /// Return reference to shared sequence number clock
                        static uint32_t & seq_clock() {
                            static uint32_t s = 0;
                            return s;
                        }

                        /// Sequence number used for this event
                        uint32_t seq;

                    public:

                        /// Constructor
                        Use32BitEventSeq() {
                            // Get sequence number from seq clock and update it
                            seq = seq_clock()++;
                        }

                        /// Length of the generated header
                        enum { header_length = 5 };

                        /*!
                         * \brief Write header
                         * \param data  Buffer to write header to
                         * \param more_headers True if there will be another header behind this one
                         */
                        void get_header(uint8_t * data, bool more_headers) {
                            if (more_headers)
                                data[0] = 0x40;
                            else
                                data[0] = 0x00;

                            *reinterpret_cast<uint32_t *>(data + 1) = htonl(seq);
                        }
                };


            } // namespace frag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __USE32BITEVENTSEQ_H_06F00D78E8653E__

