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
#include "mw/anl/Message.h"
#include "mw/nl/DistinctNL.h"
#include "mw/afp/Fragmenter.h"
#include "mw/afp/Defragmentation.h"
#include "config/type_traits/if_contains_select_type.h"
#include "mw/anl/detail/EmptyPublishParamSet.h"
#include "mw/anl/detail/EmptyEventProcessRequestPolicy.h"

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
             *  \tparam NL the network layer or a NetworkGuard, see prerequisites.
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


                    IF_CONTAINS_SELECT_TYPE_(EventProcessRequestPolicy);

                    /*! \brief  policy called on event process request
                     *
                     *  If the policy is given by the lower layer (NetworkGuard),
                     *  it is used. Otherwise (typically if there is no
                     *  NetworkGuard), an empty version is selected.
                     */
                    typedef typename if_contains_select_type_EventProcessRequestPolicy<
                                            NL,
                                            detail::EmptyEventProcessRequestPolicy
                                        >::type EventProcessRequestPolicy;


                    IF_CONTAINS_SELECT_TYPE_(PublishParamSet);

                public:

                    /*! \brief  short network representation of the subject
                     */
                    typedef typename NL::SNN SNN;

                    /*! \brief  Publish parameter set type
                     *
                     *  If a type is given by the lower layer (NetworkGuard),
                     *  it is used. Otherwise (typically if there is no
                     *  NetworkGuard), an empty version is selected.
                     */
                    typedef typename if_contains_select_type_PublishParamSet<
                                            NL,
                                            detail::EmptyPublishParamSet
                                        >::type PublishParamSet;

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
                    }

                    /*! \brief   write a message on the pluged lower network
                     *
                     *  \param[in]  snn the short network name of the subject
                     *  \param[in]  m is the message that has to be written
                     *  \param[in]  pps an optional set of special parameters
                     *              (needed for real time events)
                     *
                     *  \todo   Save copy operation in fragmentation case (needs
                     *          AFP interface extension returning AFP header and
                     *          payload separately and Packet/NL adaption)
                     */
                    void write(const SNN &snn, const Message &m, const PublishParamSet * pps = 0) {
                        TRACE_FUNCTION;
                        bool realtime = false;
                        // prepare delivery
                        if (pps) {
                            // May set network guard real time event info
                            pps->process(*this);
                            realtime = pps->realtime();
                        }
                        // deliver
                        if (m.length <= NL::info::payload) {
                            typename NL::Packet_t p(snn, m.data, m.length, realtime);
                            NL::deliver(p);
                        } else {
                            // Fragmentation using AFP (if disabled, a warning is emitted to the log)
                            typedef afp::Fragmenter<AFP_FragConfig, NL::info::payload> Frag;

                            if (m.length != (typename Frag::elen_t) m.length) {
                                ::logging::log::emit< ::logging::Warning>()
                                    << PROGMEMSTRING("AFP: Cannot publish event... too large.")
                                    << ::logging::log::endl;
                                return;
                            }

                            Frag frag(m.data, m.length);

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

                    /*! \brief  read a message from the network layer if the short network
                     *          name is equal to the short network name of the arosen packet.
                     *
                     *  \param[in]  snn the short network name of the subject
                     *  \param[out] m   The message that has to be received. If there is no message
                     *                  to read (the arosen packet was a fragment not completing a
                     *                  message), m.data is set to NULL.
                     *  \param[in]  bnl the sub network in that the event \e e will be published.
                     *
                     *  \return \li \b -1 if \e snn and the snn of the last arosen packet are different
                     *          \li \b 0 if they are equal but there is no complete message yet
                     *          \li \b 1 if they are equal and \e m contains a complete message
                     */
                    int8_t read(const SNN &snn, Message &m, const famouso::mw::nl::DistinctNL *bnl) {
                        TRACE_FUNCTION;
                        if (snn == NL::lastPacketSNN()) {
                            typename NL::Packet_t p;
                            NL::take(p);
                            if (!p.fragment) {
                                m.length = p.data_length;
                                m.data = p.data;
                                return 1;
                            } else {
                                // Apply AFP
                                afp::DefragmentationStep<AFP_DefragConfig> ds(p.data, p.data_length, snn);
                                defrag.process_fragment(ds);
                                if (ds.event_complete()) {
                                    m.data = ds.get_event_data();
                                    m.length = ds.get_event_length();
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
                        return EventProcessRequestPolicy::process(*this);
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
