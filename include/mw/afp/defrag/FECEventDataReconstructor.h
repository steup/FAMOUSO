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


#ifndef __FECEVENTDATARECONSTRUCTOR_H_3E331BC5510F68__
#define __FECEVENTDATARECONSTRUCTOR_H_3E331BC5510F68__


#if !defined(__AVR__)
// Non-AVR version

#include "debug.h"

#include "mw/afp/defrag/OutOfOrderEventDataReconstructor.h"
#include "mw/afp/defrag/detail/VarHeaderLength.h"
#include "mw/afp/shared/expandrange_cast.h"
#include "mw/afp/shared/div_round_up.h"

#include <string.h>


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {


                namespace vandermonde_fec {
                    extern "C" {
// Forward error correction library (packet erasure code based on vandermonde matrices)
#include "fec.h"
                    }
                }


                /*!
                 * \brief Policy: Reconstruct event data from fragments supporting
                 *                forward error correction.
                 *
                 * Currently not supported on AVR platform.
                 *
                 * Using this policy automatically selects FECHeaderSupport as FEC header
                 * policy.
                 *
                 * Alternative policies: OutOfOrderEventDataReconstructor, InOrderEventDataReconstructor
                 */
                template <class AFPDC>
                class FECEventDataReconstructor : public OutOfOrderEventDataReconstructor<AFPDC> {

                        typedef OutOfOrderEventDataReconstructor<AFPDC> Base;

                        typedef typename AFPDC::SizeProp::elen_t elen_t;
                        typedef typename AFPDC::SizeProp::flen_t flen_t;
                        typedef typename AFPDC::SizeProp::fcount_t fcount_t;

                        typedef class AFPDC::DefragStatistics Statistics;
                        typedef class AFPDC::Allocator Allocator;
                        typedef detail::VarHeaderLength<AFPDC> VHL;

                    public:

                        typedef FECHeaderSupport<AFPDC> FECHeaderPolicy;

                    private:

                        uint8_t ** fragments_data; ///< Array of k input data fragments (copied)
                        int * fragments_order;     ///< Array (k elements): fragment_order[n'th_put_chunk] = m'th chunk of data
                                                   ///< block (reconstruction order)

                        fcount_t k;                ///< Count of non-redundancy fragments (init: 1, no FEC mode: 0)

                        void * fec_code;           ///< Code descriptor of the used library

                        flen_t payload_length;     ///< Payload per fragment (constant, payload of first fragment)

                        /// Calculates n-k from k and redundancy level (in %)
                        static fcount_t get_redundancy_fragment_count(fcount_t k, uint8_t redundancy) {
                            return shared::div_round_up(shared::expandrange_cast(k) * redundancy,
                                                        shared::expandrange_cast((fcount_t)100));
                        }

                    public:

                        /*!
                         * \brief Constructor
                         * \param no_ext_mtu    MTU minus extension header's length
                         */
                        FECEventDataReconstructor(flen_t no_ext_mtu)
                                : Base(no_ext_mtu), fragments_data(0), fragments_order(0), k(1), fec_code(0), payload_length(0) {
                        }

                        /// Destructor
                        ~FECEventDataReconstructor() {
                            // The check cannot be done in base class. Thats why this class have to
                            // communicate with the base class to avoid double increment on
                            // statistics. Problem solved with a hack... see below.
                            if (!is_complete())
                                Statistics::event_incomplete();

                            Statistics::fragments_currently_expected_sub(Base::event_fragment_count - Base::arrived_fragment_count);

                            if (fec_code)
                                vandermonde_fec::fec_free(fec_code);

                            if (fragments_data) {
                                for (fcount_t i = 0; i < k; i++)
                                    if (fragments_data[i])
                                        Allocator::free(fragments_data[i]);
                                Allocator::free_array(fragments_data);
                            }

                            if (fragments_order)
                                Allocator::free_array(fragments_order);

                            if (Base::event_data)
                                Allocator::free(Base::event_data);

                            // Hack: give base class the information that there is a derived class
                            // - event_data will surely be always not equal to the maximum pointer value
                            // - set event_data to maximum pointer
                            Base::event_data = reinterpret_cast<uint8_t *>(-1);

                            // Base class destructor will be called...
                        }

                        /*!
                         * \brief Process new fragment
                         * \param header AFP header of the fragment
                         * \param data Payload data of the fragment
                         * \param length Payload data length of the fragment
                         */
                        void put_fragment(const Headers<AFPDC> & header, const uint8_t * data, flen_t length) {
                            FAMOUSO_ASSERT(!is_complete());       // Event should not be already complete

                            if (Base::error())                    // Do nothing on error status
                                return;

                            if (!header.fec.occurs()) {           // Enter non FEC mode
                                Base::put_fragment(header, data, length);
                                k = 0;
                                return;
                            } else if (k == 0) {                  // FEC fragment in non FEC mode -> drop
                                goto set_error;
                            }

                            if (!fragments_data) {
                                // put_fragment() called first time... init parameters from header
                                k = header.fec.get_k();
                                Base::event_fragment_count = k + get_redundancy_fragment_count(k, header.fec.get_red());
                                payload_length = Base::no_ext_mtu - VHL::get_basic_header_len(Base::event_fragment_count - 1);
                                Base::event_length = (k - 1) * payload_length + header.fec.get_len_rest();

                                Statistics::fragments_expected(Base::event_fragment_count);
                                Statistics::fragments_currently_expected_add(Base::event_fragment_count);

                                fec_code = vandermonde_fec::fec_new(k, Base::event_fragment_count);
                                fragments_data = Allocator::template alloc_array< uint8_t* >(k);
                                fragments_order = Allocator::template alloc_array<int>(k);

                                if (!fec_code || !fragments_data || !fragments_order) {
                                    ::logging::log::emit< ::logging::Warning>() << "AFP: Out of memory -> drop" << ::logging::log::endl;
                                    goto set_error;
                                }

                                memset(fragments_data, 0, k * sizeof(uint8_t *));

                                ::logging::log::emit< ::logging::Info>() << "AFP: FEC: payload length " << ::logging::log::dec << (unsigned int)payload_length << ", k " << (unsigned int)k << ", n " << (unsigned int)Base::event_fragment_count << ::logging::log::endl;
                            }

                            // Wrong MTU will lead to undefined behaviour...
                            // Only last non-redundancy fragment may be smaller than maximum
                            {
                                FOR_FAMOUSO_ASSERT_ONLY(bool wrong_mtu_or_frag_len = length != payload_length && header.fseq != Base::event_fragment_count - k);
                                FAMOUSO_ASSERT(!wrong_mtu_or_frag_len);
                            }

                            // Collect data
                            fragments_data[Base::arrived_fragment_count] = Allocator::alloc(payload_length);
                            if (!fragments_data[Base::arrived_fragment_count]) {
                                // Do not set error... may be reconstructable although we have to drop this fragment
                                ::logging::log::emit< ::logging::Warning>() << "AFP: Out of memory -> drop" << ::logging::log::endl;
                                return;
                            }

                            memcpy(fragments_data[Base::arrived_fragment_count], data, length);
                            if (payload_length > length)
                                memset(fragments_data[Base::arrived_fragment_count] + length, 0, payload_length - length);

                            fragments_order[Base::arrived_fragment_count] = Base::event_fragment_count - header.fseq - 1;

                            Base::arrived_fragment_count++;

                            Statistics::fragments_currently_expected_sub(1);
                            Statistics::fragment_used();

                            if (is_complete())
                                Statistics::event_complete();

                            return;

                        set_error:
                            Base::no_ext_mtu = 0;
                            return;
                        }


                        /*!
                         *  \brief  Returns whether the event can be reconstructed completely.
                         */
                        bool is_complete() {
                            // In non FEC mode ask base class
                            if (k == 0)
                                return Base::is_complete();

                            // If we have less than k fragments, reconstruction is not possible (yet).
                            return Base::arrived_fragment_count >= k && !Base::error();
                        }

                        /*!
                         * \brief Returns the defragmented data block
                         * \pre is_complete() returns true
                         */
                        uint8_t * get_data() {
                            // In non FEC mode ask base class
                            if (k == 0)
                                return Base::get_data();

                            FAMOUSO_ASSERT(is_complete());

                            if (!Base::event_data) {
                                // Decode data
                                int err = vandermonde_fec::fec_decode(fec_code,
                                                                      reinterpret_cast<void **>(
                                                                          const_cast<uint8_t **>(fragments_data)
                                                                      ),
                                                                      fragments_order,
                                                                      payload_length);
                                if (err != 0) {
                                    ::logging::log::emit< ::logging::Warning>() << "AFP: FEC decode error -> drop" << ::logging::log::endl;
                                    goto set_error;
                                }

                                // Copy to one output buffer
                                Base::event_data = Allocator::alloc(Base::event_length);
                                if (!Base::event_data) {
                                    ::logging::log::emit< ::logging::Warning>() << "AFP: Out of memory -> drop" << ::logging::log::endl;
                                    goto set_error;
                                }

                                uint8_t * p = Base::event_data;
                                fcount_t km1 =  k - 1;
                                for (fcount_t i = 0; i < km1; i++, p += payload_length)
                                    memcpy(p, fragments_data[i], payload_length);
                                memcpy(p, fragments_data[km1], Base::event_length - (km1 * payload_length));
                            }

                            return Base::event_data;

                        set_error:
                            Base::no_ext_mtu = 0;
                            return 0;
                        }

                        /*!
                         * \brief Returns the length of the event's data.
                         * \pre is_complete() returns true
                         */
                        elen_t get_length() {
                            FAMOUSO_ASSERT(is_complete());
                            return Base::event_length;
                        }
                };


            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#else
// AVR version

#include "boost/mpl/assert.hpp"

namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {
                template <class AFPDC>
                class FECEventDataReconstructor {
                    BOOST_MPL_ASSERT_MSG(false, FEC_not_supported_on_AVR, ());
                };
            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso

#endif


#endif // __FECEVENTDATARECONSTRUCTOR_H_3E331BC5510F68__

