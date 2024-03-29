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


#ifndef __USENOEVENTSEQ_H_B3235E551DCB93__
#define __USENOEVENTSEQ_H_B3235E551DCB93__


namespace famouso {
    namespace mw {
        namespace afp {
            namespace frag {


                /*!
                 * \brief Policy: Don't use event sequence numbering
                 *
                 * Should be only used if there is no (or nearly no) packet loss and
                 * no reordering.
                 *
                 * Alternative policies: Use32BitEventSeq
                 */
                template <class AFPC>
                class UseNoEventSeq {

                    public:

                        /// Length of the generated header
                        enum { header_length = 0 };

                        /*!
                         * \brief Write header (does nothing)
                         * \param data  Buffer to write header to
                         * \param more_headers True if there will be another header behind this one
                         */
                        void get_header(uint8_t * data, bool more_headers) {
                        }

                };


            } // namespace frag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __USENOEVENTSEQ_H_B3235E551DCB93__

