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


#ifndef __NOEVENTSEQHEADERSUPPORT_H_12E7A7F82C6455__
#define __NOEVENTSEQHEADERSUPPORT_H_12E7A7F82C6455__


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {


                /*!
                 * \brief Policy: Do not support event sequence number extension header
                 *
                 * Fragments containing an event sequence number header will be dropped.
                 * This policy is chosen automatically if SingleEventDemux or
                 * MultiSourceDemux is used as demultiplexing policy.
                 *
                 * Alternative policies: EventSeqHeaderSupport
                 */
                template <class DCP>
                class NoEventSeqHeaderSupport {

                        typedef typename DCP::SizeProp::flen_t   flen_t;

                    public:

                        /*!
                         * \brief Return whether this header can be found at the beginning
                         *         of data.
                         */
                        bool check(const uint8_t * data) {
                            return false;
                        }

                        /*!
                         * \brief Read header from the beginning of data.
                         *
                         * \returns Length of the header. Zero on error.
                         */
                        flen_t read_header(const uint8_t * data) {
                            return 0;
                        }

                        /*!
                         * \brief Returns whether this header was read.
                         */
                        bool occurs() const {
                            return false;
                        }
                };


            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __NOEVENTSEQHEADERSUPPORT_H_12E7A7F82C6455__

