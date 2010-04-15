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


#ifndef __USEFEC_H_C5A7E8776574FE__
#define __USEFEC_H_C5A7E8776574FE__


#if !defined(__AVR__)
// Non-AVR version

#include "debug.h"
#include "util/endianness.h"

#include "mw/afp/shared/expandrange_cast.h"
#include "mw/afp/shared/div_round_up.h"

#include <string.h>


namespace famouso {
    namespace mw {
        namespace afp {
            namespace frag {


                namespace vandermonde_fec {
                    extern "C" {
// Forward error correction library (packet erasure code based on vandermonde matrices)
#include "fec.h"
                    }
                }


                /*!
                 * \brief Fragmentation with forward error correction
                 *
                 * Currently not supported on AVR platform.
                 *
                 * TODO Error handling (new), allocator
                 */
                template <class AFPFC>
                class UseFEC {

                        typedef typename AFPFC::SizeProp::elen_t   elen_t;
                        typedef typename AFPFC::SizeProp::flen_t   flen_t;
                        typedef typename AFPFC::SizeProp::fcount_t fcount_t;

                        typedef typename AFPFC::FECRedundancy Redundancy;
                        typedef typename AFPFC::Allocator Allocator;


                        /// Length of event to be fragmented
                        elen_t event_length;

                        /// Count of message/data/non-redundancy fragments
                        fcount_t k;

                        /// Count of all fragments
                        fcount_t n;

                        /// Array of k pointers pointing to input fragments
                        const uint8_t ** nonred_fragments;

                        /// Array index of next fragment (valid between 0 and n-1)
                        fcount_t next_fragment_idx;

                        /// true if nonred_fragment[k-1] is padded with zeros, because it is shorter than payload_length bytes
                        bool smaller_fragment;

                        /// Length of the maximum payload of one fragment
                        flen_t payload_length;

                        /// Code descriptor of the used library
                        void * fec_code;

                        /// FEC header structure
                        struct __attribute__((packed)) Header1001 {
                            uint8_t head;           ///< Bits: e01? 1001 - e: one more extension header; ?: don't care
                            uint8_t red;            ///< Redundancy
                            uint32_t k;             ///< Count of data packets
                            uint16_t len_rest;      ///< Length of the last (non-redundancy) fragment
                        };

                        /// Header len_rest in endian byte order (saved to avoid multiple calculation)
                        uint16_t header_len_rest;


                        /// Calculates n-k from k and redundancy level (in %)
                        static fcount_t get_redundancy_fragment_count(fcount_t k, uint8_t redundancy) {
                            return shared::div_round_up(shared::expandrange_cast(k) * redundancy,
                                                        shared::expandrange_cast((fcount_t)100));
                        }

                    public:

                        /// Length of the generated header
                        enum { header_length = sizeof(Header1001) };


                        /*!
                         * \brief Calculate total number of fragments (data + redundancy)
                         * \param k Number of data fragments
                         * \returns Total number of fragments
                         */
                        static fcount_t k2n(fcount_t k) {
                            return k + get_redundancy_fragment_count(k, Redundancy::value());
                        }

                        /// Constructor
                        UseFEC() :
                                nonred_fragments(0), next_fragment_idx(0),
                                smaller_fragment(false), fec_code(0) {
                        }

                        /// Destructor
                        ~UseFEC() {
                            if (smaller_fragment)
                                Allocator::free(const_cast<uint8_t *>(nonred_fragments[k-1]));
                            if (fec_code)
                                vandermonde_fec::fec_free(fec_code);
                            if (nonred_fragments)
                                Allocator::free_array(nonred_fragments);
                        }

                        /*!
                         * \brief Initialization. Has to be called immediately after construction.
                         * \param event_length Length of the event data
                         * \param payload_length Maximum length of payload
                         * \param k  Count of data fragments
                         */
                        void init(elen_t event_length, flen_t payload_length, fcount_t k) {
                            FAMOUSO_ASSERT(payload_length > 0);

                            this->event_length = event_length;
                            this->k = k;
                            this->n = k2n(k);
                            this->payload_length = payload_length;
                            this->nonred_fragments = Allocator::template alloc_array<const uint8_t *>(k);
                            FAMOUSO_ASSERT(nonred_fragments);

                            ::logging::log::emit< ::logging::Info>() << "AFP: Fragmenter FEC: " << ::logging::log::dec << (unsigned long)k << " data fragments, " << (unsigned long)(n - k) << " redundancy fragments (from " << (unsigned long)Redundancy::value() << "% red)" << ::logging::log::endl;

                            // Following is very expensive for big k and n... in case many events with
                            // same k and n are sent, it would be nice to have fec_code sharing (low priority TODO)
                            fec_code = vandermonde_fec::fec_new(k, n);
                            FAMOUSO_ASSERT(fec_code);

                            header_len_rest = htons(((event_length - 1) % payload_length) + 1);
                        }

                        /*!
                         * \brief Write header
                         * \param data  Buffer to write header to
                         * \param more_headers True if there will be another header behind this one
                         */
                        void get_header(uint8_t * data, bool more_headers) {
                            Header1001 * h = reinterpret_cast<Header1001 *>(data);
                            h->head = (more_headers ? 0x40 | 0x29 : 0x29);
                            h->red = Redundancy::value();
                            h->k = htonl(k);
                            h->len_rest = header_len_rest;
                        }


                        /*!
                         * \brief Give payload data of message fragment to FEC data
                         *        constructor.
                         * \param data Pointer to fragment payload. Has to stay valid
                         *        as long as get_red_fragment() may be called.
                         */
                        void put_nonred_fragment(const uint8_t * data) {
                            FAMOUSO_ASSERT(next_fragment_idx < k);
                            nonred_fragments[next_fragment_idx++] = data;
                        }

                        /*!
                         * \brief Give payload data of last message fragment to FEC
                         *        data collector. This fragment may be shorter than
                         *        payload_length bytes.
                         * \param data Pointer to fragment payload. Has to stay valid
                         *        as long as get_red_fragment() may be called.
                         * \param length Length of data.
                         */
                        void put_smaller_nonred_fragment(const uint8_t * data, flen_t length) {
                            FAMOUSO_ASSERT(next_fragment_idx == k - 1);
                            FAMOUSO_ASSERT(length < payload_length);
                            FAMOUSO_ASSERT(event_length % payload_length == length);

                            // Make it same size for encoding library and pad with zero.
                            uint8_t * last_nonred_frag = Allocator::alloc(payload_length);
                            FAMOUSO_ASSERT(nonred_fragments);
                            memcpy(last_nonred_frag, data, length);
                            memset(last_nonred_frag + length, 0, payload_length - length);

                            nonred_fragments[next_fragment_idx++] = last_nonred_frag;
                            smaller_fragment = true;
                        }

                        /*!
                         * \brief Get redundancy fragment payload data.
                         * \param data Pointer to buffer to fill with data.
                         */
                        void get_red_fragment(uint8_t * data) {
                            FAMOUSO_ASSERT(next_fragment_idx < n);

                            vandermonde_fec::fec_encode(fec_code,
                                                        reinterpret_cast<void **>(
                                                            const_cast<uint8_t **>(nonred_fragments)
                                                        ),
                                                        data,
                                                        next_fragment_idx++,
                                                        payload_length);
                        }
                };


            } // namespace frag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#else
// AVR version

#include "boost/mpl/assert.hpp"

namespace famouso {
    namespace mw {
        namespace afp {
            namespace frag {
                template <class AFPFC>
                class UseFEC {
                    BOOST_MPL_ASSERT_MSG(false, FEC_not_supported_on_AVR, ());
                };
            } // namespace frag
        } // namespace afp
    } // namespace mw
} // namespace famouso

#endif


#endif // __USEFEC_H_C5A7E8776574FE__

