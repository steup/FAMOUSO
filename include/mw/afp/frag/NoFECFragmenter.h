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


#ifndef __NOFECFRAGMENTER_H_248EC79C4F9347__
#define __NOFECFRAGMENTER_H_248EC79C4F9347__


#include "debug.h"

#include "mw/afp/shared/div_round_up.h"
#include "config/type_traits/ExpandedRangeTypeSelector.h"

#include "boost/mpl/assert.hpp"

// memcpy
#include <string.h>



namespace famouso {
    namespace mw {
        namespace afp {
            namespace frag {


                /*!
                 * \brief Fragmentation not supporting FEC
                 *
                 * All fragments will be of equal size (MTU), the last fragment may be smaller. Due to
                 * variable header length the payload is variable too.
                 */
                template < class FCP >
                class NoFECFragmenter {

                        typedef typename FCP::SizeProp::elen_t   elen_t;
                        typedef typename FCP::SizeProp::flen_t   flen_t;
                        typedef typename FCP::SizeProp::fcount_t fcount_t;

                        typedef class FCP::EventSeqUsagePolicy EventSeqPolicy;
                        typedef class FCP::OverflowErrorChecking OverflowErrorCheck;

                    protected:

                        /// Current length of the AFP basic header
                        flen_t basic_header_length;

                        /// Current length of the payload
                        flen_t payload_length;

                        /// Data remaining to be fragmented
                        const uint8_t * remaining_data;

                        /// Length of the data remaining to be fragmented (without FEC data fragments)
                        elen_t remaining_length;


                        /// Number of fragments still to be fetched via get_fragment()
                        fcount_t remaining_fragments;

                        /// Next fragement sequence number for decrementing header length
                        fcount_t next_header_shorten_fseq;

                        /// Is next fragment is the first fragment?
                        bool first_fragment;


                        /// Event sequence number generator (may do nothing)
                        EventSeqPolicy event_seq;

                        /// Error indication
                        bool err;


                    public:

                        /// Minimum header length
                        enum { min_header_length = 1 + EventSeqPolicy::header_length };

                        /// Extension header's length
                        enum { extension_header_length = EventSeqPolicy::header_length };

                        /*!
                         * \brief Init a new fragmenter for an event
                         * \param[in] event_data Buffer containing the data to be fragmented
                         * \param[in] event_length Length of event_data buffer
                         * \param[in] mtu  Maximum length of constructed fragments (header + payload)
                         */
                        void init(const uint8_t * event_data, elen_t event_length, flen_t mtu) {
                            // Check if MTU is big enough (may be optimized away)
                            if (mtu <= min_header_length) {
                                err = true;
                                ::logging::log::emit< ::logging::Error>()
                                    << "AFP: MTU too small for selected features."
                                    << ::logging::log::endl;
                                return;
                            }

                            remaining_length = event_length;

                            err = false;
                            OverflowErrorCheck overflow_err;

                            // Find header and payload size and the number of fragments
                            elen_t rem_length = event_length;
                            fcount_t frag_count = 0;
                            flen_t header_length = min_header_length;
                            fcount_t curr_hl_frag_count = 32;
                            fcount_t max_frag_count = 32;

                            payload_length = mtu - header_length;
                            typedef typename ExpandedRangeTypeSelector<elen_t>::type eelen_t;
                            eelen_t curr_hl_payload = (eelen_t)curr_hl_frag_count * (eelen_t)payload_length;
                            overflow_err.check_equal(curr_hl_payload / payload_length, curr_hl_frag_count);

                            while (rem_length > curr_hl_payload &&
                                   !overflow_err.error()) {
                                rem_length -= curr_hl_payload;
                                overflow_err.check_smaller(frag_count, frag_count + curr_hl_frag_count);
                                frag_count += curr_hl_frag_count;
                                header_length++;
                                payload_length--;

                                if (header_length >= mtu)
                                    break;

                                overflow_err.check_smaller(curr_hl_frag_count, 127 * curr_hl_frag_count);
                                curr_hl_frag_count = 127 * max_frag_count;

                                overflow_err.check_smaller(max_frag_count, max_frag_count << 7);
                                max_frag_count <<= 7;

                                curr_hl_payload = (eelen_t)curr_hl_frag_count * (eelen_t)payload_length;
                                overflow_err.check_equal(curr_hl_payload / payload_length, curr_hl_frag_count);
                            }

                            if (header_length >= mtu || overflow_err.error()) {
                                err = true;
                                ::logging::log::emit< ::logging::Error>() << "AFP: Event of size "
                                    << ::logging::log::dec << static_cast<uint64_t>(event_length)
                                    << " could not be fragmented with this configuration (too large)."
                                    << ::logging::log::endl;
                                return;
                            }

                            frag_count += shared::div_round_up(rem_length, (elen_t)payload_length);

                            ::logging::log::emit< ::logging::Info>() << "AFP: Fragmenter (no FEC): "
                                << ::logging::log::dec << static_cast<uint64_t>(remaining_length)
                                << " bytes data -> " << static_cast<uint64_t>(payload_length)
                                << " bytes payload in " << static_cast<uint64_t>(remaining_fragments)
                                << " fragments" << ::logging::log::endl;

                            FAMOUSO_ASSERT(frag_count > 0);
                            remaining_fragments = frag_count;
                            next_header_shorten_fseq = get_next_header_shorten_fseq(frag_count-1);

                            basic_header_length = header_length - extension_header_length;
                            first_fragment = true;
                            remaining_data = event_data;
                        }


                        /*!
                         * \brief Returns whether an error occured.
                         *
                         * Check this after construction.
                         */
                        bool error() {
                            return err;
                        }

                        /*!
                         * \brief Write next fragment to fragment_data buffer.
                         * \param[in] fragment_data Output buffer for fragment (at least mtu Bytes)
                         * \returns Length of data put into fragment buffer
                         */
                        flen_t get_fragment(uint8_t * fragment_data) {
                            if (!remaining_fragments)
                                return 0;

                            remaining_fragments--;

                            // Reduce header length? (and increase payload?)
                            if (remaining_fragments == next_header_shorten_fseq) {
                                basic_header_length--;
                                payload_length++;
                                next_header_shorten_fseq = get_next_header_shorten_fseq(remaining_fragments);
                                FAMOUSO_ASSERT(basic_header_length > 0);
                            }

                            // Insert header
                            flen_t fragment_length = get_header(fragment_data);
                            fragment_data += fragment_length;

                            // Insert payload
                            FAMOUSO_ASSERT(remaining_length > 0);
                            if (remaining_length >= payload_length) {

                                // Get full data fragment
                                memcpy(fragment_data, remaining_data, payload_length);
                                fragment_length += payload_length;

                                remaining_length -= payload_length;
                                remaining_data += payload_length;

                            } else {

                                // Get smaller data fragment (smaller than max. payload)
                                memcpy(fragment_data, remaining_data, remaining_length);
                                fragment_length += remaining_length;

                                remaining_length = 0;
                            }

                            return fragment_length;
                        }


                    protected:

                        /*!
                         *  \brief  Returns next fseq number for which header length decrements
                         *  \param  fseq    Current fragment sequence number
                         */
                        fcount_t get_next_header_shorten_fseq(fcount_t fseq) {

                            if (fseq <= 31)
                                return (fcount_t)-1;

                            if (sizeof(fcount_t) == 1) {
                                // One byte version
                                return 31;
                            } else if (sizeof(fcount_t) == 2) {
                                // Two byte version
                                if (fseq <= 4095)
                                    return 31;
                                else
                                    return (fcount_t)4095;
                            } else {
                                // Generic version
                                uint8_t c = 0;

                                do {
                                    fseq >>= 7;
                                    c++;
                                } while (fseq > 31);

                                return (((fcount_t)1) << (7*c - 2)) - 1;
                            }
                        }


                        /*!
                         * \brief Write current fragment's header into buffer.
                         * \param[in]  data Data buffer
                         * \param[in]  more_ext_hdr True, if a further extension header will be appended
                         * \returns Header length
                         */
                        flen_t get_header(uint8_t * data, bool more_ext_hdr = false) {
                            // Write basic header
                            {
                                fcount_t fseq = remaining_fragments;

                                // TODO: support 8 byte basic header if (sizeof(fcount_t) > 4)

                                // Fragment sequence number
                                flen_t i = basic_header_length;
                                while (i > 0) {
                                    data[--i] = fseq & 0xff;
                                    fseq >>= 8;
                                }

                                // Chaining
                                uint8_t flags = (EventSeqPolicy::header_length || more_ext_hdr) ? 0x2 : 0;

                                // First fragment
                                if (first_fragment) {
                                    flags |= 0x1;
                                    first_fragment = false;
                                }


                                // Header length + write flags
                                FAMOUSO_ASSERT(basic_header_length <= 6);
                                FAMOUSO_ASSERT((data[0] & (0xff << (6 - basic_header_length))) == 0);
                                data[0] |= ((0xf8 | flags) << (6 - basic_header_length));
                            }
                            data += basic_header_length;

                            // Write extension header: event sequence number
                            event_seq.get_header(data, more_ext_hdr);
                            data += EventSeqPolicy::header_length;

                            return basic_header_length + EventSeqPolicy::header_length;
                        }
                };


            } // namespace frag
        } // namespace afp
    } // namespace mw
} // namespace famouso



#endif // __NOFECFRAGMENTER_H_248EC79C4F9347__

