/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __VARHEADERLENGTH_H_FB8B6AC493785D__
#define __VARHEADERLENGTH_H_FB8B6AC493785D__


#include "debug.h"
#include "config/type_traits/ExpandedRangeTypeSelector.h"


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {
                namespace detail {


                    template <class DCP>
                    class VarHeaderLength {

                            typedef typename DCP::SizeProp::elen_t elen_t;
                            typedef typename DCP::SizeProp::flen_t flen_t;
                            typedef typename DCP::SizeProp::fcount_t fcount_t;

                        public:

                            /// Return type of get_payload()
                            typedef typename ExpandedRangeTypeSelector<elen_t>::type eelen_t;

                            /*!
                             *  \brief  Returns maximum payload of an event
                             *  \param  frag_count  Count of fragments the event consists of
                             *  \param  no_ext_mtu  MTU minus extension header's length
                             *  \returns    Maximum payload of an event with given parameters (may exceed the
                             *              range of elen_t)
                             *
                             *  This function assumes variable basic header length, constant extension header length
                             *  and variable payload per fragment (the full MTU is used except for the last fragment).
                             */
                            static eelen_t get_payload(fcount_t frag_count, flen_t no_ext_mtu) {
                                eelen_t p = 0;          // payload
                                flen_t h = 1;           // header length
                                fcount_t c_h = 32;      // max. fragment count for header length = h
                                fcount_t c_max = 32;    // max. fragment count for header length <= h

                                while (frag_count > c_h) {
                                    frag_count -= c_h;
                                    p += c_h * (no_ext_mtu - h);

                                    h++;
                                    FAMOUSO_ASSERT(h < no_ext_mtu);
                                    c_h = 127 * c_max;
                                    c_max = 128 * c_max;
                                }
                                p += frag_count * (no_ext_mtu - h);
                                return p;
                            }


                            /*!
                             *  \brief  Returns the basic header length for a given fragment sequence number
                             *  \param  fseq    Fragment sequence number
                             *  \return Basic header length
                             *
                             *  Assumes variable basic header length. Chooses minimal header length sufficient
                             *  to encode \p fseq.
                             */
                            static flen_t get_basic_header_len(fcount_t fseq) {
                                flen_t hl = 1;
                                while (fseq >= 32) {
                                    hl++;
                                    fseq >>= 7;
                                }
                                return hl;
                            }
                    };


                } // namespace detail
            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __VARHEADERLENGTH_H_FB8B6AC493785D__

