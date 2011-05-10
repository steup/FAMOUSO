/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *               2010-2011 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __EventLayer_h__
#define __EventLayer_h__

#include "debug.h"

#include "mw/common/Subject.h"
#include "mw/common/Event.h"
#include "mw/api/EventChannel.h"
#include "mw/api/detail/NoChannelTrampoline.h"
#include "mw/nl/DistinctNL.h"
#include "mw/el/EventDispatcher.h"
#include "mw/el/EventLayerCallBack.h"


namespace famouso {
    namespace mw {
        namespace el {

            template <typename LL, typename EL, typename Config>
            struct NoManagementLayer {};

            /// Default management layer config
            struct DefaultConfig {
                enum {
                    /// Whether to support real time publisher event channels
                    support_real_time_publisher = 1,

                    /// Whether there are real time channels in the network and a subscriber on this node
                    support_real_time_subscriber = 1,

                    /// Whether to support forwarding on gateways
                    support_forwarding = 1
                };
            };

            template <class LL,
                      template <class, class, class> class ManLayPolicy = NoManagementLayer,
                      class Config = DefaultConfig>
            class EventLayer;

            /// Configurator for the sublayers of the event layer
            template <class LL, template <class, class, class> class ManLayPolicy, class Config>
            class EventSublayerConfigurator {
                    typedef EventLayer<LL, ManLayPolicy, Config> EL;
                public:
                    typedef ManLayPolicy <
                                famouso::mw::el::EventDispatcher<
                                    LL,
                                    EL
                                >,
                                EL,
                                Config
                            > type;
            };

            /// Configurator for the sublayers of the event layer (omits management layer)
            template <class LL, class Config>
            class EventSublayerConfigurator<LL, NoManagementLayer, Config> {
                    typedef EventLayer<LL, NoManagementLayer, Config> EL;
                public:
                    typedef famouso::mw::el::EventDispatcher<
                                LL,
                                EL
                            > type;
            };

            /*! \brief The event layer provides the main publish/subscribe functionality
             *
             *          It has the ability to get different lower layer plugs and supports
             *          local as well as remote publishing. Furthermore, it handles all
             *          relevant data structure for the management of event channels.
             *          The event layer is structured in sublayers. The main
             *          publish/subscribe functionality is provided by the
             *          EventDispatcher. An optional sublayer needed for real time and
             *          other Quality-of-Service aspects is The ManagementLayer.
             *
             *  \tparam LL the lower layer is a configurable component see prerequisites.
             *  \tparam ManLayPolicy the management layer policy is a configurable component.
             *          Pass the type ml::ManagementLayer to include the management layer,
             *          otherwise it is omitted.
             *  \tparam Config Management layer configuration
             *
             *  \pre    The type of template parameter LL can be an famouso::mw::anl:AbstractNetworkLayer
             *          or an famouso::mw::nal::NetworkAdapter dependent on the configuration of the
             *          middleware stack
             */
            template <class LL, template <class, class, class> class ManLayPolicy, class Config>
            class EventLayer : public EventSublayerConfigurator<LL, ManLayPolicy, Config>::type {

                    /// First sublayer
                    typedef typename EventSublayerConfigurator<LL, ManLayPolicy, Config>::type SL;

                    /// Event channel type
                    typedef famouso::mw::api::EventChannel<EventLayer> EC;

                public:
                    /*! \brief  short network representation of the subject
                     */
                    typedef typename LL::SNN SNN;

                    /*! \brief  Publish parameter set type
                     */
                    typedef typename SL::PublishParamSet PublishParamSet;

                    /*! \brief  self type
                     */
                    typedef EventLayer type;

                    /*! \brief  Channel trampoline policy
                     */
                    typedef typename SL::ChannelTrampolinePolicy ChannelTrampolinePolicy;


                    /*! \brief  initialize the middleware core
                     */
                    void init() {
                        IncommingEventFromNL.bind<type, &type::fetch>(this);
                        SL::init();
                    }

                    /*! \brief  announce an event channel on all lower layers
                     *
                     *  \param[in]  ec the publishing event channel that is announced
                     */
                    void announce(EC & ec) {
                        TRACE_FUNCTION;
                        SL::announce(ec);
                    }

                    /*! \brief  unannounce an event channel
                     *
                     *  \param[in]  ec the unannounced event channel
                     */
                    void unannounce(EC & ec) {
                        TRACE_FUNCTION;
                        SL::unannounce(ec);
                    }

                    /*! \brief  subscribe an event channel on all lower layers
                     *
                     *  \param[in]  ec the subscribing event channel that is announced
                     */
                    void subscribe(EC & ec) {
                        TRACE_FUNCTION;
                        SL::subscribe(ec);
                    }

                    /*! \brief  unsubscribe an event channel
                     *
                     *  \param[in]  ec the unsubscribed event channel
                     */
                    void unsubscribe(EC & ec) {
                        TRACE_FUNCTION;
                        SL::unsubscribe(ec);
                    }

                    /*! \brief  publish the given event within an event channel
                     *
                     *  \param[in]  ec the publishing event channel
                     *  \param[in]  e the event that is published
                     *  \param[in]  pps publish parameter set
                     */
                    void publish(const EC &ec, const Event &e, const PublishParamSet * pps = 0) {
                        TRACE_FUNCTION;
                        SL::publish(ec, e, pps);
                    }

                    /*! \brief  publishes an event from an event channel locally to other
                     *          event channels that have subscribed to the same subject.
                     *          This method can be seen as the local event channel
                     *          handler.
                     *
                     *  \param[in]  e the event to publish
                     *
                     */
                    void publish_local(const Event &e) {
                        TRACE_FUNCTION;
                        SL::publish_local(e);
                    }


                    /*! \brief Fetches an event from a specific sub network and
                     *         publishes it locally.
                     *
                     *  \param[in]  bnl the network from where an event fetching was requested.
                     *
                     *  \todo       The default value of zero describes that we have no fetching
                     *              request from a lower layer. This is for further enhancements
                     *              like fetching in a time-triggered system, where we fetch
                     *              periodically without interrupt support from lower network
                     *              layer.
                     */
                    void fetch(famouso::mw::nl::DistinctNL *bnl = 0) {
                        TRACE_FUNCTION;
                        // Inform low layer about fetching starts. Lower layers may
                        // cancel fetching, e.g. if the incoming packet contained
                        // internal management information that is processed during
                        // the event_process_request() function call
                        if (!LL::event_process_request(bnl))
                            return;

                        SL::fetch(bnl);

                        // inform lower layer that we are done
                        LL::event_processed();
                    }
            };


        } // namespace el
    } // namespace mw
} // namespace famouso

#endif /* __EventLayer_h__ */

