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

#include "mw/afp/defrag/detail/VarHeaderLength.h"


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
                template <class DCP>
                class InOrderEventDataReconstructor {

                        typedef typename DCP::SizeProp::elen_t elen_t;
                        typedef typename DCP::SizeProp::flen_t flen_t;
                        typedef typename DCP::SizeProp::fcount_t fcount_t;

                        typedef class DCP::DefragStatistics Statistics;
                        typedef class DCP::Allocator Allocator;
                        typedef detail::VarHeaderLength<DCP> VHL;

                    public:

                        typedef NoFECHeaderSupport<DCP> FECHeaderPolicy;

                    private:

                        uint8_t * event_data;             ///< Restored data block

                        elen_t event_length;              ///< Length of the currently restored data (sum of put fragment's length)

                        fcount_t event_fragment_count;    ///< Count of fragments the event consists of

                        fcount_t arrived_fragment_count;  ///< Count of data chunks collected.

                        flen_t no_ext_mtu;                ///< MTU minus extension header's length. Zero encodes error status of this object.

                    public:

                        /*!
                         * \brief Constructor
                         * \param no_ext_mtu    MTU minus extension header's length
                         */
                        InOrderEventDataReconstructor(flen_t no_ext_mtu)
                                : event_data(0), event_length(0),
                                event_fragment_count(0),
                                arrived_fragment_count(0),
                                no_ext_mtu(no_ext_mtu) {
                        }

                        /// Destructor
                        ~InOrderEventDataReconstructor() {
                            if (!is_complete())
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
                        void put_fragment(const Headers<DCP> & header, const uint8_t * data, flen_t length) {
                            // Only last fragment may be smaller than maximum
                            FOR_FAMOUSO_ASSERT_ONLY(bool wrong_mtu_or_frag_len = length != no_ext_mtu - VHL::get_basic_header_len(header.fseq) && header.fseq != 0);
                            FAMOUSO_ASSERT(!wrong_mtu_or_frag_len); // Wrong MTU will lead to undefined behaviour
                            FAMOUSO_ASSERT(!is_complete());         // Event should not be already complete

                            if (no_ext_mtu == 0)            // Do nothing on error status
                                return;
                            if (header.fec.occurs()) {      // Cannot handle events using FEC -> error
                                ::logging::log::emit< ::logging::Warning>()
                                    << PROGMEMSTRING("AFP: FEC not supported -> drop")
                                    << ::logging::log::endl;
                                goto set_error;
                            }

                            if (!event_data) {
                                // First fragment
                                if (!header.first_fragment)
                                    goto set_error;

                                event_fragment_count = header.fseq + 1;

                                event_data = Allocator::alloc(VHL::get_payload(event_fragment_count, no_ext_mtu));
                                if (!event_data) {
                                    ::logging::log::emit< ::logging::Warning>()
                                        << PROGMEMSTRING("AFP: Out of memory -> drop")
                                        << ::logging::log::endl;
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

                                memcpy(event_data + event_length, data, length);

                                Statistics::fragments_currently_expected_sub(1);
                            }

                            arrived_fragment_count++;
                            event_length += length;

                            Statistics::fragment_used();

                            if (is_complete())
                                Statistics::event_complete();

                            return;

                        set_error:
                            no_ext_mtu = 0;
                            return;
                        }

                        /*!
                         *  \brief  Returns whether the event already got reconstructed completely.
                         */
                        bool is_complete() {
                            return get_data();
                        }

                        /*!
                         * \brief Returns the defragmented data block
                         * \pre is_complete() returns true
                         */
                        uint8_t * get_data() {
                            // To avoid an extra condition checking for initialization case this function is
                            // used to implement is_complete(). To achieve this the behaviour goes beyonds
                            // the specification of get_data(): it returns NULL in case of an incomplete
                            // event.
                            return (arrived_fragment_count == event_fragment_count && !error()) ? event_data : 0;
                        }

                        /*!
                         * \brief Returns the length of the event's data.
                         * \pre is_complete() returns true
                         */
                        elen_t get_length() {
                            FAMOUSO_ASSERT(is_complete());
                            return event_length;
                        }

                        /*!
                         * \brief Returns true if an error occured and event cannot be reconstructed for sure.
                         */
                        bool error() {
                            return no_ext_mtu == 0;
                        }
                };


            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __INORDEREVENTDATARECONSTRUCTOR_H_3E331BC5510F68__

