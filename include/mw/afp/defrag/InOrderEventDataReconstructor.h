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


#ifndef __INORDEREVENTDATARECONSTRUCTOR_H_3E331BC5510F68__
#define __INORDEREVENTDATARECONSTRUCTOR_H_3E331BC5510F68__


#include "debug.h"


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {


                /*!
                 * \brief Policy: Reconstruct event data from fragments arriving in
                 *                order. Does not support forward error correction.
                 *
                 * Using this policy automatically selects NoFECHeaderSupport as FEC header
                 * policy.
                 *
                 * Alternative policies: OutOfOrderEventDataReconstructor, FECEventDataReconstructor
                 */
                template <class AFPDC>
                class InOrderEventDataReconstructor {

                        typedef typename AFPDC::SizeProp::elen_t elen_t;
                        typedef typename AFPDC::SizeProp::flen_t flen_t;
                        typedef typename AFPDC::SizeProp::fcount_t fcount_t;

                        typedef class AFPDC::DefragStatistics Statistics;
                        typedef class AFPDC::Allocator Allocator;

                    public:

                        typedef NoFECHeaderSupport<AFPDC> FECHeaderPolicy;

                    private:

                        uint8_t * event_data;             ///< Restored data block

                        elen_t event_length;              ///< Length of the currently restored data (sum of put fragment's length)

                        fcount_t event_fragment_count;    ///< Count of fragments the event consists of

                        fcount_t arrived_fragment_count;  ///< Count of data chunks collected.

                        flen_t max_fragment_length;       ///< Maximum fragment payload length (MTU - header). Zero encodes error status of this object.

                    public:

                        /*!
                         * \brief Constructor
                         * \param max_chunk_length Maximum payload of fragment (MTU - header)
                         */
                        InOrderEventDataReconstructor(flen_t max_chunk_length)
                                : event_data(0), event_length(0),
                                event_fragment_count(0),
                                arrived_fragment_count(0),
                                max_fragment_length(max_chunk_length) {
                        }

                        /// Destructor
                        ~InOrderEventDataReconstructor() {
                            if (get_data() == 0)
                                Statistics::event_incomplete();

                            Statistics::fragments_currently_expected_sub(event_fragment_count - arrived_fragment_count);

                            if (event_data)
                                Allocator::free(event_data);
                        }

                        /*!
                         * \brief Process new fragment
                         * \param header AFP header of the fragment
                         * \param data Payload data of the fragment
                         * \param length Payload data length of the fragment
                         */
                        void put_fragment(const Headers<AFPDC> & header, const uint8_t * data, flen_t length) {
                            // Only last fragment may be smaller than maximum
                            FOR_FAMOUSO_ASSERT_ONLY(bool wrong_mtu = length != max_fragment_length && header.fseq != 0);
                            FAMOUSO_ASSERT(!wrong_mtu);             // Wrong MTU will lead to undefined behaviour
                            FAMOUSO_ASSERT(!get_data());            // Event should not be already complete

                            if (max_fragment_length == 0)   // Do nothing on error status
                                return;
                            if (header.fec.occurs()) {      // Cannot handle events using FEC -> error
                                ::logging::log::emit< ::logging::Warning>() << "AFP: FEC not supported -> drop" << ::logging::log::endl;
                                goto set_error;
                            }

                            if (!event_data) {
                                // First fragment
                                if (!header.first_fragment)
                                    goto set_error;

                                event_fragment_count = header.fseq + 1;

                                event_data = Allocator::alloc(event_fragment_count * max_fragment_length);
                                if (!event_data) {
                                    ::logging::log::emit< ::logging::Warning>() << "AFP: Out of memory -> drop" << ::logging::log::endl;
                                    goto set_error;
                                }

                                memcpy(event_data, data, length);

                                Statistics::fragments_expected(event_fragment_count);
                                Statistics::fragments_currently_expected_add(event_fragment_count - 1);
                            } else {
                                // Later fragment
                                fcount_t seq_upcount = event_fragment_count - header.fseq - 1;

                                if (seq_upcount != arrived_fragment_count)  // Packet loss or out of order -> error
                                    goto set_error;

                                memcpy(event_data + seq_upcount * max_fragment_length, data, length);

                                Statistics::fragments_currently_expected_sub(1);
                            }

                            arrived_fragment_count++;
                            event_length += length;

                            Statistics::fragment_used();

                            if (get_data() != 0)
                                Statistics::event_complete();

                            return;

                        set_error:
                            max_fragment_length = 0;
                            return;
                        }

                        /*!
                         * \brief Returns the defragmented data block if event is complete, NULL otherwise.
                         */
                        uint8_t * get_data() {
                            return (arrived_fragment_count == event_fragment_count && !error()) ? event_data : 0;
                        }

                        /*!
                         * \brief Returns the length of the event's data. Only valid if get_data() != 0.
                         */
                        elen_t get_length() {
                            return event_length;
                        }

                        /*!
                         * \brief Returns true if an error occured and event cannot be reconstructed for sure.
                         */
                        bool error() {
                            return max_fragment_length == 0;
                        }
                };


            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __INORDEREVENTDATARECONSTRUCTOR_H_3E331BC5510F68__

