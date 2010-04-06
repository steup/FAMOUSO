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


#ifndef __FRAGMENTER_H_248EC79C4F9347__
#define __FRAGMENTER_H_248EC79C4F9347__


#include "debug.h"

#include "mw/afp/shared/div_round_up.h"

#include "boost/mpl/assert.hpp"

// memcpy
#include <string.h>


namespace famouso {
    namespace mw {
        namespace afp {


            /*!
             * \brief Splits a block of data into several pieces (fragments)
             *
             * All fragments will be of equal size, last non-redundancy fragment may be smaller.
             * Can be used for Abstract Network Layer and Application Layer (or wherever you want ^^)
             *
             * \tparam AFPFC AFPConfig
             * \tparam mtu_compile_time Maximum length of constructed fragments (header + payload).
             *                          Setting this value enables compile time checking if the MTU
             *                          is to small for headers.
             */
            template < class AFPFC, unsigned int mtu_compile_time = 0 >
            class Fragmenter {

                    typedef typename AFPFC::SizeProp::elen_t   elen_t;
                    typedef typename AFPFC::SizeProp::flen_t   flen_t;
                    typedef typename AFPFC::SizeProp::fcount_t fcount_t;

                    typedef class AFPFC::EventSeqUsagePolicy EventSeqPolicy;
                    typedef class AFPFC::FECUsagePolicy      FECPolicy;
                    typedef class AFPFC::OverflowErrorChecking OverflowErrorCheck;

                protected:

                    /// Length of the AFP basic header
                    flen_t basic_header_length;

                    /// Length of the payload
                    flen_t payload_length;


                    /// Data remaining to be fragmented
                    const uint8_t * remaining_data;

                    /// Length of the data remaining to be fragmented (without FEC data fragments)
                    elen_t remaining_length;


                    /// Number of fragments still to be fetched via get_fragment()
                    fcount_t remaining_fragments;

                    /// Is next fragment is the first fragment?
                    bool first_fragment;


                    /// Event sequence number generator (may do nothing)
                    EventSeqPolicy event_seq;

                    /// Forward error correction (may do nothing)
                    FECPolicy fec;

                    /// Error indication
                    bool err;


                    /*!
                     * \brief Init a new fragmenter for an event (called by constructors)
                     * \param[in] event_data Buffer containing the data to be fragmented
                     * \param[in] event_length Length of event_data buffer
                     * \param[in] mtu  Maximum length of constructed fragments (header + payload)
                     */
                    void init(const uint8_t * event_data, elen_t event_length, flen_t mtu) {
                        err = false;
                        OverflowErrorCheck overflow_err;

                        // Find header and payload size and the number of fragments

                        flen_t header_length = 1 + EventSeqPolicy::header_length + FECPolicy::header_length;
                        payload_length = mtu - header_length;

                        elen_t k = shared::div_round_up(event_length, (elen_t)payload_length);
                        overflow_err.check_equal(k, (fcount_t)k);

                        remaining_fragments = FECPolicy::k2n(k);
                        overflow_err.check_smaller_or_equal(k, remaining_fragments);

                        fcount_t max_fragments = 32;

                        while (remaining_fragments > max_fragments &&
                                !overflow_err.error()) {
                            header_length++;
                            payload_length--;

                            if (header_length >= mtu)
                                break;

                            k = shared::div_round_up(event_length, (elen_t)payload_length);
                            overflow_err.check_equal(k, (fcount_t)k);

                            remaining_fragments = FECPolicy::k2n(k);
                            overflow_err.check_smaller_or_equal(k, remaining_fragments);

                            overflow_err.check_smaller(max_fragments, max_fragments << 7);
                            max_fragments <<= 7;
                        }

                        ::logging::log::emit< ::logging::Info>() << "AFP: Fragmenter: " << ::logging::log::dec << (unsigned long)event_length << " bytes data -> " << (unsigned long)payload_length << " bytes payload in " << (unsigned long)remaining_fragments << " fragments" << ::logging::log::endl;

                        if (header_length >= mtu || overflow_err.error()) {
                            err = true;
                            ::logging::log::emit< ::logging::Error>() << "AFP: Event of size " << ::logging::log::dec << (unsigned long)event_length << " could not be fragmented with this configuration. Changing SizeProperties config should help." << ::logging::log::endl;
                            return;
                        }

                        basic_header_length = header_length - EventSeqPolicy::header_length - FECPolicy::header_length;
                        fec.init(event_length, payload_length, k);

                        remaining_data = event_data;
                        remaining_length = event_length;

                        first_fragment = true;
                    }

                public:

                    /*!
                     * \brief Construct a new fragmenter for an event (if mtu_compile_time template parameter is set)
                     * \param[in] event_data Buffer containing the data to be fragmented
                     * \param[in] event_length Length of event_data buffer
                     */
                    Fragmenter(const uint8_t * event_data, elen_t event_length) {
                        BOOST_MPL_ASSERT_MSG(mtu_compile_time > 1 + EventSeqPolicy::header_length + FECPolicy::header_length,
                                             MTU_is_too_small_for_this_configuration, ());
                        init(event_data, event_length, mtu_compile_time);
                    }

                    /*!
                     * \brief Construct a new fragmenter for an event
                     * \param[in] event_data Buffer containing the data to be fragmented
                     * \param[in] event_length Length of event_data buffer
                     * \param[in] mtu  Maximum length of constructed fragments (header + payload)
                     */
                    Fragmenter(const uint8_t * event_data, elen_t event_length, flen_t mtu) {
                        if (mtu <= 1 + EventSeqPolicy::header_length + FECPolicy::header_length) {
                            err = true;
                            ::logging::log::emit< ::logging::Error>() << "AFP: MTU too small for selected features." << ::logging::log::endl;
                            return;
                        }
                        init(event_data, event_length, mtu);
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

                        // Insert header
                        flen_t fragment_length = get_header(fragment_data);
                        fragment_data += fragment_length;

                        // Insert payload
                        if (remaining_length == 0) {

                            // Done with data fragments, get FEC redundancy fragment
                            fec.get_red_fragment(fragment_data);
                            fragment_length += payload_length;

                        } else if (remaining_length >= payload_length) {

                            // Get full data fragment
                            memcpy(fragment_data, remaining_data, payload_length);
                            fec.put_nonred_fragment(remaining_data);
                            fragment_length += payload_length;

                            remaining_length -= payload_length;
                            remaining_data += payload_length;

                        } else {

                            // Get smaller data fragment (smaller than max. payload)
                            memcpy(fragment_data, remaining_data, remaining_length);
                            fec.put_smaller_nonred_fragment(remaining_data, remaining_length);
                            fragment_length += remaining_length;

                            remaining_length = 0;
                        }

                        return fragment_length;
                    }

                private:
                    /*!
                     * \brief Write current fragment's header into buffer.
                     * \param[in]  Data buffer
                     * \returns Header length
                     */
                    flen_t get_header(uint8_t * data) {
                        // Write basic header
                        {
                            // Fragment sequence number
                            fcount_t fseq = remaining_fragments;
                            flen_t i = basic_header_length;
                            while (i > 0) {
                                data[--i] = fseq & 0xff;
                                fseq >>= 8;
                            }

                            // Chaining
                            uint8_t flags = (EventSeqPolicy::header_length || FECPolicy::header_length) ? 0x2 : 0;

                            // First fragment
                            if (first_fragment) {
                                flags |= 0x1;
                                first_fragment = false;
                            }

                            // Header length + write flags
                            data[0] |= ((0xf8 | flags) << (6 - basic_header_length));
                        }
                        data += basic_header_length;

                        // Write extension header: event sequence number
                        event_seq.get_header(data, FECPolicy::header_length);
                        data += EventSeqPolicy::header_length;

                        // Write extension header: forward error correction
                        fec.get_header(data, false);

                        return basic_header_length + EventSeqPolicy::header_length + FECPolicy::header_length;
                    }
            };


        } // namespace afp
    } // namespace mw
} // namespace famouso



#endif // __FRAGMENTER_H_248EC79C4F9347__

