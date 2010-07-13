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

#include "assert/staticerror.h"

#include "mw/afp/Config.h"
#include "mw/afp/FragPolicySelector.h"


namespace famouso {
    namespace mw {
        namespace afp {


            /*!
             * \brief Splits a block of data into several pieces (fragments)
             *
             * All fragments will be of equal size, last non-redundancy fragment may be smaller.
             * Can be used for Abstract Network Layer and Application Layer (or wherever you want ^^)
             *
             * \tparam AFPFC AFP Fragmenter Config
             * \tparam mtu_compile_time Maximum length of constructed fragments (header + payload).
             *                          Setting this value enables compile time checking if the MTU
             *                          is to small for headers.
             */
            template < class AFPFC, unsigned int mtu_compile_time = 0 >
            class Fragmenter {

                    typedef FragPolicySelector<AFPFC> FCP;

                    typedef class FCP::FragImplPolicy FragmenterImpl;

                public:

                    typedef typename FCP::SizeProp::elen_t   elen_t;
                    typedef typename FCP::SizeProp::flen_t   flen_t;
                    typedef typename FCP::SizeProp::fcount_t fcount_t;

                protected:

                    /// Implementation policy (differs for FEC/non-FEC)
                    FragmenterImpl frag;

                public:

                    /*!
                     * \brief Construct a new fragmenter for an event (if mtu_compile_time template parameter is set)
                     * \param[in] event_data Buffer containing the data to be fragmented
                     * \param[in] event_length Length of event_data buffer
                     */
                    Fragmenter(const uint8_t * event_data, elen_t event_length) {
                        // Compile time check if minimum header fits into MTU
                        FAMOUSO_STATIC_ASSERT_ERROR(mtu_compile_time > FragmenterImpl::min_header_length,
                                                    MTU_is_too_small_for_this_configuration, ());
                        frag.init(event_data, event_length, mtu_compile_time);
                    }

                    /*!
                     * \brief Construct a new fragmenter for an event
                     * \param[in] event_data Buffer containing the data to be fragmented
                     * \param[in] event_length Length of event_data buffer
                     * \param[in] mtu  Maximum length of constructed fragments (header + payload)
                     */
                    Fragmenter(const uint8_t * event_data, elen_t event_length, flen_t mtu) {
                        frag.init(event_data, event_length, mtu);
                    }

                    /*!
                     * \brief Returns whether an error occured.
                     *
                     * Check this after construction.
                     */
                    bool error() {
                        return frag.error();
                    }

                    /*!
                     * \brief Write next fragment to fragment_data buffer.
                     * \param[in] fragment_data Output buffer for fragment (at least mtu Bytes)
                     * \returns Length of data put into fragment buffer
                     */
                    flen_t get_fragment(uint8_t * fragment_data) {
                        return frag.get_fragment(fragment_data);
                    }
            };


            /*!
             *  \brief  Empty version of Fragmenter for disabling fragmentation in the ANL
             */
            template < unsigned int mtu_compile_time >
            class Fragmenter<Disable, mtu_compile_time> {

                public:

                    typedef uint16_t elen_t;
                    typedef uint16_t flen_t;
                    typedef uint16_t fcount_t;

                    /*!
                     * \brief Construct a new fragmenter for an event (if mtu_compile_time template parameter is set)
                     * \param[in] event_data Buffer containing the data to be fragmented
                     * \param[in] event_length Length of event_data buffer
                     */
                    Fragmenter(const uint8_t * event_data, elen_t event_length) {
                        ::logging::log::emit< ::logging::Warning>()
                            << PROGMEMSTRING("Event is to big to deliver at once and fragmentation is disabled.")
                            << ::logging::log::endl;
                    }

                    /*!
                     * \brief Construct a new fragmenter for an event
                     * \param[in] event_data Buffer containing the data to be fragmented
                     * \param[in] event_length Length of event_data buffer
                     * \param[in] mtu  Maximum length of constructed fragments (header + payload)
                     */
                    Fragmenter(const uint8_t * event_data, elen_t event_length, flen_t mtu) {
                        ::logging::log::emit< ::logging::Warning>()
                            << PROGMEMSTRING("Event is to big to deliver at once and fragmentation is disabled.")
                            << ::logging::log::endl;
                    }

                    /*!
                     * \brief Returns whether an error occured.
                     *
                     * Check this after construction.
                     */
                    bool error() {
                        return true;
                    }

                    /*!
                     * \brief Write next fragment to fragment_data buffer.
                     * \param[in] fragment_data Output buffer for fragment (at least mtu Bytes)
                     * \returns Length of data put into fragment buffer
                     */
                    flen_t get_fragment(uint8_t * fragment_data) {
                        return 0;
                    }
            };

        } // namespace afp
    } // namespace mw
} // namespace famouso



#endif // __FRAGMENTER_H_248EC79C4F9347__

