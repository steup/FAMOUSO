/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __AbstractNetworkLayer_h__
#define __AbstractNetworkLayer_h__

#include "debug.h"
#include "mw/common/Event.h"
#include "mw/nl/DistinctNL.h"
#include "mw/afp/Fragmenter.h"
#include "mw/afp/Defragmentation.h"

namespace famouso {
    namespace mw {
        namespace anl {

            /*! \brief The abstract network layer provides functionality that is not
             *         specific to a concrete network layer.
             *
             *          It has the ability to get different lower layer plugs and supports
             *          functionality like fragmentation, or in later versions, it handles
             *          also the aspects of quality of service or attribute management.
             *
             *  \tparam NL the network layer see prerequisites.
             *  \tparam AFP_FragConfig  the AFP fragmentation %config, see \ref afp_config
             *  \tparam AFP_DefragConfig  the AFP defragmentation %config, see \ref afp_config
             *
             *  \pre    The type of template parameters can be an famouso::mw::nl::CANNL,
             *          famouso::mw::nl::AWDSNL, famouso::mw::nl::UDPMultiCastNL,
             *          famouso::mw::nl::UDPBroadCastNL or an famouso::mw::nl::VoidNL dependent
             *          on the %configuration of the middleware stack
             */
            template < class NL, class AFP_FragConfig = typename NL::AFP_Config, class AFP_DefragConfig = typename NL::AFP_Config >
            class AbstractNetworkLayer : public NL {

                    /*! \brief  defragmentation data and algorithms
                     */
                    afp::DefragmentationProcessorANL<AFP_DefragConfig> defrag;

                public:

                    /*! \brief  short network representation of the subject
                     */
                    typedef typename NL::SNN SNN;

                    /*! \brief  short network representation of the subscribe subject
                     *          that is used for announcing subscribtion network-wide
                     */
                    SNN   subscribe_SNN;

                    /*! \brief  constructor
                     */
                    AbstractNetworkLayer() :
                       defrag(NL::info::payload) {
                    }

                    /*! \brief Initalizes the sub networks and bind the subscription
                     *         management channel.
                     */
                    void init() {
                        NL::init();
                        Subject s("SUBSCRIBE");
                        NL::bind(s, subscribe_SNN);
                    }

                    /*! \brief  announce a subject and get its short network representation
                     *
                     *  \param[in]   s the subject that is announced
                     *  \param[out]  snn the short network name of the subject
                     */
                    void announce(const Subject &s, SNN &snn) {
                        TRACE_FUNCTION;
                        NL::bind(s, snn);
                        // nach dem Bind noch bekannt geben,
                        // das dieser Kanal publiziert wird
                    }

                    /*! \brief   publish an event on the pluged lower network
                     *
                     *  \param[in]  snn the short network name of the subject
                     *  \param[in]  e the event that has to be published
                     *
                     *  \todo   Save copy operation in fragmentation case (needs
                     *          AFP interface extension returning AFP header and
                     *          payload separately and Packet/NL adaption)
                     */
                    void publish(const SNN &snn, const Event &e) {
                        TRACE_FUNCTION;
                        if (e.length <= NL::info::payload) {
                            typename NL::Packet_t p(snn, &e[0], e.length);
                            NL::deliver(p);
                        } else {
                            // Fragmentation using AFP (if disabled, a warning is emitted to the log)
                            typedef afp::Fragmenter<AFP_FragConfig, NL::info::payload> Frag;

                            if (e.length != (typename Frag::elen_t) e.length) {
                                ::logging::log::emit< ::logging::Warning>()
                                    << PROGMEMSTRING("AFP: Cannot publish event... too large.")
                                    << ::logging::log::endl;
                                return;
                            }

                            Frag frag(e.data, e.length);

                            if (frag.error())
                                return;

                            uint8_t buffer [NL::info::payload];
                            typename Frag::flen_t length;
                            while ( (length = frag.get_fragment(buffer)) ) {
                                typename NL::Packet_t p(snn, buffer, length, true);
                                NL::deliver(p);
                            }
                        }
                    }

                    /*! \brief  subscribe a subject and get its short network representation
                     *
                     *          The subscribtion contains out of two parts. First it binds
                     *          the subject to its short network name, and second it announces
                     *          the subscription to allow gateways the establishing of forwarding
                     *          channels.
                     *
                     *  \param[in]   s the subject that is announced
                     *  \param[out]  snn the short network name of the subject
                     */
                    void subscribe(const Subject &s, SNN &snn) {
                        TRACE_FUNCTION;
                        NL::bind(s, snn);
                        // nach dem Bind auch noch bekannt geben,
                        // dass dieser Kanal subscribiert wird
                        typename NL::Packet_t p(subscribe_SNN, const_cast<uint8_t*>(s.tab()), 8);
                        NL::deliver(p);
                    }


                    /*! \brief  fetches an event from the network layer if the short network
                     *          name is equal to the short network name of the arosen packet.
                     *
                     *  \param[in]  snn the short network name of the subject
                     *  \param[out] e   The event that has to be published. If there is no event to
                     *                  fetch (the arosen packet was a fragment not completing an
                     *                  event), e.data is set to NULL.
                     *  \param[in]  bnl the sub network in that the event \e e will be published.
                     *
                     *  \return \li \b -1 if \e snn and the snn of the last arosen packet are different
                     *          \li \b 0 if they are equal but there is no complete event to fetch
                     *          \li \b 1 if they are equal and \e e contains a complete event
                     */
                    int8_t fetch(const SNN &snn, Event &e, const famouso::mw::nl::DistinctNL *bnl) {
                        TRACE_FUNCTION;
                        if (snn == NL::lastPacketSNN()) {
                            typename NL::Packet_t p;
                            NL::fetch(p);
                            if (!p.fragment) {
                                e.length = p.data_length;
                                e.data = p.data;
                                return 1;
                            } else {
                                // Apply AFP
                                afp::DefragmentationStep<AFP_DefragConfig> ds(p.data, p.data_length, snn);
                                defrag.process_fragment(ds);
                                if (ds.event_complete()) {
                                    e.data = ds.get_event_data();
                                    e.length = ds.get_event_length();
                                    return 1;
                                } else {
                                    return 0;
                                }
                            }
                        } else {
                            return -1;
                        }
                    }

                    /*! \brief Is called by the higher layer to signalise that
                     *         an event processing request was arised.
                     *
                     *  \param[in]  bnl the sub network-ID from where the request came.
                     *  \return     Returns whether to continue with event processing.
                     */
                    bool event_process_request(famouso::mw::nl::DistinctNL * const bnl) {
                        return true;
                    }

                    /*! \brief Is called by the higher layer to signalise that
                     *         the event is processed now.
                     */
                    void event_processed() {
                        defrag.last_event_processed();
                    }

            };

        } // namespace anl
    } // namespace mw
} //namespace famouso

#endif /* __AbstractNetworkLayer_h__ */
