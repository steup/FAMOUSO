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


#ifndef __USENOFEC_H_C5A7E8776574FE__
#define __USENOFEC_H_C5A7E8776574FE__


namespace famouso {
    namespace mw {
        namespace afp {
            namespace frag {


                /*!
                 * \brief Policy: Fragmentation without forward error correction
                 *
                 * Alternative policies: UseFEC
                 */
                template <class AFPFC>
                class UseNoFEC {

                        typedef typename AFPFC::SizeProp::elen_t   elen_t;
                        typedef typename AFPFC::SizeProp::flen_t   flen_t;
                        typedef typename AFPFC::SizeProp::fcount_t fcount_t;

                    public:

                        /// Length of the generated header
                        enum { header_length = 0 };

                        /*!
                         * \brief Calculate total number of fragments (non-redundancy + redundancy)
                         * \param k Number of non-redunancy fragments
                         * \returns Total number of fragments
                         */
                        static fcount_t k2n(fcount_t k) {
                            return k;
                        }

                        /// Constructor
                        UseNoFEC() {
                        }

                        /*!
                         * \brief Initialization. Has to be called immediately after construction.
                         * \param event_length Length of the event data
                         * \param payload_length Maximum length of payload
                         * \param k  Count of data fragments
                         */
                        void init(elen_t event_length, flen_t payload_length, fcount_t k) {
                        }

                        /*!
                         * \brief Write header (does nothing)
                         * \param data  Buffer to write header to
                         * \param more_headers True if there will be another header behind this one
                         */
                        void get_header(uint8_t * data, bool more_headers) {
                        }

                        /*!
                         * \brief Give payload data of message fragment to FEC data
                         *  constructor.
                         * \param data Pointer to fragment payload. Has to stay valid
                         *   as long as get_red_fragment() may be called.
                         */
                        void put_nonred_fragment(const uint8_t * data) {
                        }

                        /*!
                         * \brief Give payload data of last message fragment to FEC
                         *  data collector. This fragment may be shorter than
                         *  payload_length bytes.
                         * \param data Pointer to fragment payload. Has to stay valid
                         *   as long as get_red_fragment() may be called.
                         * \param length Length of data.
                         */
                        void put_smaller_nonred_fragment(const uint8_t * data, flen_t length) {
                        }

                        /*!
                         * \brief Get redundancy fragment payload data.
                         * \param data Pointer to buffer to fill with data.
                         */
                        void get_red_fragment(uint8_t * data) {
                        }
                };


            } // namespace frag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __USENOFEC_H_C5A7E8776574FE__

