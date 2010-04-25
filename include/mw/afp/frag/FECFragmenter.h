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


#ifndef __FECFRAGMENTER_H_248EC79C4F9347__
#define __FECFRAGMENTER_H_248EC79C4F9347__


#include "debug.h"

#include "mw/afp/frag/NoFECFragmenter.h"
#include "mw/afp/frag/UseFEC.h"
#include "mw/afp/shared/div_round_up.h"

#include "boost/mpl/assert.hpp"

// memcpy
#include <string.h>


namespace famouso {
    namespace mw {
        namespace afp {
            namespace frag {


                /*!
                 * \brief   Fragmentation supporting generation of forward error correction redundancy
                 *
                 * For all fragments the payload length is constant (the last  non-redundancy fragment
                 * may be smaller), because the used FEC algorithm needs equal block size. As the
                 * header length may vary between fragments, the full MTU is not used in many cases.
                 */
                template < class FCP >
                class FECFragmenter : public NoFECFragmenter<FCP> {

                        typedef NoFECFragmenter<FCP> Base;

                        typedef typename FCP::SizeProp::elen_t   elen_t;
                        typedef typename FCP::SizeProp::flen_t   flen_t;
                        typedef typename FCP::SizeProp::fcount_t fcount_t;

                        typedef class FCP::OverflowErrorChecking OverflowErrorCheck;
                        typedef UseFEC<FCP> FEC;

                    protected:

                        /// FEC redundancy generator
                        FEC fec;

                        /// Will this event contain FEC redundancy?
                        bool redundancy;

                    public:

                        /// Minimum header length
                        enum { min_header_length = Base::min_header_length + FEC::header_length };

                        /// Extension header's length
                        enum { extension_header_length = Base::extension_header_length + FEC::header_length };

                        /*!
                         * \brief Init a new fragmenter for an event
                         * \param[in] event_data Buffer containing the data to be fragmented
                         * \param[in] event_length Length of event_data buffer
                         * \param[in] mtu  Maximum length of constructed fragments (header + payload)
                         */
                        void init(const uint8_t * event_data, elen_t event_length, flen_t mtu) {
                            // Check if MTU is big enough (may be optimized away)
                            if (mtu <= min_header_length) {
                                Base::err = true;
                                ::logging::log::emit< ::logging::Error>() << "AFP: MTU too small for selected features." << ::logging::log::endl;
                                return;
                            }

                            Base::err = false;
                            OverflowErrorCheck overflow_err;

                            // Find header and payload size and the number of fragments

                            flen_t header_length = min_header_length;
                            flen_t payload_length = mtu - header_length;

                            // FEC library supports only even paylaod lengths
                            if (payload_length & 1)
                                payload_length--;

                            elen_t k = shared::div_round_up(event_length, (elen_t)payload_length);
                            overflow_err.check_equal(k, (fcount_t)k);

                            fcount_t frag_count = FEC::k2n(k);
                            overflow_err.check_smaller_or_equal(k, frag_count);

                            fcount_t max_fragments = 32;

                            while (frag_count > max_fragments &&
                                    !overflow_err.error()) {
                                header_length++;
                                payload_length = mtu - header_length;
                                if (payload_length & 1)
                                    payload_length--;

                                if (header_length >= mtu)
                                    break;

                                k = shared::div_round_up(event_length, (elen_t)payload_length);
                                overflow_err.check_equal(k, (fcount_t)k);

                                frag_count = FEC::k2n(k);
                                overflow_err.check_smaller_or_equal(k, frag_count);

                                overflow_err.check_smaller(max_fragments, max_fragments << 7);
                                max_fragments <<= 7;
                            }

                            if (frag_count == k) {
                                // No redundancy fragment -> use base class
                                redundancy = false;
                                Base::init(event_data, event_length, mtu);
                                return;
                            } else {
                                redundancy = true;
                            }

                            ::logging::log::emit< ::logging::Info>() << "AFP: Fragmenter: " << ::logging::log::dec << (unsigned long)event_length << " bytes data -> " << (unsigned long)payload_length << " bytes payload in " << (unsigned long)frag_count << " fragments" << ::logging::log::endl;

                            if (header_length >= mtu || overflow_err.error()) {
                                Base::err = true;
                                ::logging::log::emit< ::logging::Error>() << "AFP: Event of size " << ::logging::log::dec << (unsigned long)event_length << " could not be fragmented with this configuration. Changing SizeProperties config should help." << ::logging::log::endl;
                                return;
                            }

                            FAMOUSO_ASSERT(frag_count > 0);
                            Base::remaining_fragments = frag_count;
                            Base::next_header_shorten_fseq = get_next_header_shorten_fseq(frag_count-1);

                            Base::payload_length = payload_length;
                            Base::first_fragment = true;
                            Base::basic_header_length = header_length - extension_header_length;
                            Base::remaining_data = event_data;
                            Base::remaining_length = event_length;

                            fec.init(event_length, payload_length, k);
                        }

                    public:

                        /*!
                         * \brief Write next fragment to fragment_data buffer.
                         * \param[in] fragment_data Output buffer for fragment (at least mtu Bytes)
                         * \returns Length of data put into fragment buffer
                         */
                        flen_t get_fragment(uint8_t * fragment_data) {
                            if (!redundancy)
                                return Base::get_fragment(fragment_data);

                            if (!Base::remaining_fragments)
                                return 0;

                            Base::remaining_fragments--;

                            // Reduce header length?
                            if (Base::remaining_fragments == Base::next_header_shorten_fseq) {
                                Base::basic_header_length--;
                                Base::next_header_shorten_fseq = Base::get_next_header_shorten_fseq(Base::remaining_fragments);
                                FAMOUSO_ASSERT(Base::basic_header_length > 0);
                            }

                            // Insert header (basic and non-FEC extension headers)
                            flen_t fragment_length = Base::get_header(fragment_data, true);
                            fragment_data += fragment_length;

                            // Insert FEC extension header
                            fec.get_header(fragment_data, false);
                            fragment_length += FEC::header_length;
                            fragment_data += FEC::header_length;

                            // Insert payload
                            if (Base::remaining_length == 0) {

                                // Done with data fragments, get FEC redundancy fragment
                                fec.get_red_fragment(fragment_data);
                                fragment_length += Base::payload_length;

                            } else if (Base::remaining_length >= Base::payload_length) {

                                // Get full data fragment
                                memcpy(fragment_data, Base::remaining_data, Base::payload_length);
                                fec.put_nonred_fragment(Base::remaining_data);
                                fragment_length += Base::payload_length;

                                Base::remaining_length -= Base::payload_length;
                                Base::remaining_data += Base::payload_length;

                            } else {

                                // Get smaller data fragment (smaller than max. payload)
                                memcpy(fragment_data, Base::remaining_data, Base::remaining_length);
                                fec.put_smaller_nonred_fragment(Base::remaining_data, Base::remaining_length);
                                fragment_length += Base::remaining_length;

                                Base::remaining_length = 0;
                            }

                            return fragment_length;
                        }

                };


            } // namespace frag
        } // namespace afp
    } // namespace mw
} // namespace famouso



#endif // __FECFRAGMENTER_H_248EC79C4F9347__

