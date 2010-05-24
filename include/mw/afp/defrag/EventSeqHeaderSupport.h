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


#ifndef __EVENTSEQHEADERSUPPORT_H_7D736FC2E006DD__
#define __EVENTSEQHEADERSUPPORT_H_7D736FC2E006DD__


#include "util/endianness.h"


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {


                /*!
                 * \brief Policy: Support event sequence number extension header
                 *
                 * This policy is chosen automatically if EventSeqDemux is used
                 * as demultiplexing policy.
                 *
                 * Alternatice policies: NoEventSeqHeaderSupport
                 */
                template <class DCP>
                class EventSeqHeaderSupport {

                        typedef typename DCP::SizeProp::flen_t   flen_t;

                        /// true if the value of seq is valid
                        bool seq_valid;

                        /// Event sequence number
                        uint32_t seq;

                    public:

                        /// Constructor
                        EventSeqHeaderSupport() :
                                seq_valid(false), seq(0) {
                        }

                        /*!
                         * \brief Return whether this header can be found at the beginning
                         *        of data.
                         */
                        bool check(const uint8_t * data) {
                            return (*data & 0xbf) == 0;
                        }

                        /*!
                         * \brief Read header from the beginning of data.
                         *
                         * \returns Length of the header. Zero on error.
                         */
                        flen_t read_header(const uint8_t * data) {
                            seq = ntohl(*reinterpret_cast<const uint32_t *>(data + 1));
                            seq_valid = true;
                            return 5;
                        }

                        /*!
                         * \brief Returns whether this header was read.
                         */
                        bool occurs() const {
                            return seq_valid;
                        }


                        /*!
                         * \brief Returns the event seqence number read from the header.
                         *
                         * Should be only called if occured() returns true.
                         */
                        uint32_t get_eseq() const {
                            return seq;
                        }
                };


            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __EVENTSEQHEADERSUPPORT_H_7D736FC2E006DD__

