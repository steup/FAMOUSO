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

#ifndef __EVENTDISPATCHER_H_284D09B9BACF24__
#define __EVENTDISPATCHER_H_284D09B9BACF24__

#include "debug.h"

#include "mw/common/Subject.h"
#include "mw/common/Event.h"
#include "mw/api/SubscriberEventChannel.h"
#include "mw/api/EventChannel.h"
#include "mw/nl/DistinctNL.h"

#include "object/Queue.h"

namespace famouso {
    namespace mw {
        namespace el {

            /*! \brief The event dispatcher provides the main publish/subscribe functionality
             *
             *          It is a sublayer of the event layer. It supports local as well as remote
             *          publishing. Furthermore, it handles all relevant data structure for the
             *          management of event channels.
             *
             *  \tparam LL the lower layer is a configurable component see prerequisites.
             *  \tparam EL the event layer type
             *
             *  \pre    The type of template parameters can be an famouso::mw::anl:AbstractNetworkLayer
             *          or an famouso::mw::nal::NetworkAdapter dependent on the configuration of the
             *          middleware stack
             */
            template < class LL, class EL >
            class EventDispatcher : public LL {

                protected:

                    typedef LL LowerLayer;

                    typedef famouso::mw::api::EventChannel<EL> EC;

                    // potentiell gefaerdete Datenstrukturen
                    // wegen Nebenlaeufigkeit

                    /*! \brief data structure for managing publisher event channels
                     */
                    Queue Publisher;

                    /*! \brief data structure for managing subscriber event channels
                     */
                    Queue Subscriber;

                public:

                    /*! \brief  short network representation of the subject
                     */
                    typedef typename LL::SNN SNN;

                    /*! \brief  Publish parameter set type
                     */
                    typedef typename LL::PublishParamSet PublishParamSet;

                    /*! \brief  Channel trampoline policy: no trampoline needed
                     *
                     *  This may be overwritten by the ManagementLayer.
                     */
                    typedef famouso::mw::api::detail::NoChannelTrampoline ChannelTrampolinePolicy;

                    /*! \brief  self type
                     */
                    typedef EventDispatcher type;

                    /*! \brief  short network representation of the subscribe subject
                     *          that is used for announcing subscribtion network-wide
                     *
                     *  \todo   this functionality has to be provided by a policy of
                     *          the management layer
                     */
                    SNN   subscribe_SNN;

                    /*! \brief  initialize the middleware core */
                    void init() {
                        // initialization code
                        LL::init();
                        /*  \todo   this functionality has to be provided by a policy of
                         *          the management layer
                         */
                        Subject s("SUBSCRIBE");
                        LL::bind(s, subscribe_SNN);
                    }


                    /*! \brief  announce an event channel on all lower layers and register
                     *          the event channel on a local data structure for publisher
                     *
                     *  \param[in]  ec the publishing event channel that is announced
                     */
                    void announce(EC &ec) {
                        TRACE_FUNCTION;
                        LL::bind(ec.subject(), ec.snn());
                        Publisher.append(ec);
                    }

                    /*! \brief  publishing within an event channel the given event
                     *
                     *  \param[in]  ec the publishing event channel
                     *  \param[in]  e the event that is published
                     *  \param[in]  pps publish parameter set
                     *
                     */
                    void publish(const EC &ec, const Event &e, const PublishParamSet * pps) {
                        TRACE_FUNCTION;
                        ::logging::log::emit< ::logging::Info>()
                            << PROGMEMSTRING("Publish channel with addr=")
                            << ::logging::log::hex
                            << reinterpret_cast<const void*>(&ec)
                            << PROGMEMSTRING(" with Subject -> [")
                            << ec.subject().value() << ']'
                            << ::logging::log::endl;
                        // publish on all  lower layers/subnets
                        LL::write(ec.snn(), e, pps);

                        // publish locally on all subscribed event channels
                        static_cast<EL*>(this)->publish_local(e);
                    }

                    /*! \brief  subscribe an event channel on all lower layers and register
                     *          the event channel on a local data structure for subscribers
                     *
                     *  \param[in]  ec the subscribing event channel that is announced
                     */
                    void subscribe(EC &ec) {
                        TRACE_FUNCTION;
                        ::logging::log::emit< ::logging::Info>()
                            << PROGMEMSTRING("Subscribe channel with addr=")
                            << ::logging::log::hex
                            << reinterpret_cast<const void*>(&ec)
                            << PROGMEMSTRING(" with Subject -> [")
                            << ec.subject().value() << ']'
                            << ::logging::log::endl;
                        LL::bind(ec.subject(), ec.snn());
                        Subscriber.append(ec);
                        // nach dem Bind auch noch bekannt geben,
                        // dass dieser Kanal subscribiert wird
                        /*  \todo   this functionality has to be provided by a policy of
                         *          the management layer
                         */
                        Event e(ec.subject());
                        e.data=const_cast<uint8_t*>(ec.subject().tab());
                        e.length=8;
                        LL::write(subscribe_SNN,e);
                    }

                    /*! \brief  unsubscribe an event channel and deregister the event
                     *          channel from the local data structure for subscribers
                     *
                     *  \param[in]  ec the unsubscribed event channel
                     */
                    void unsubscribe(EC &ec) {
                        TRACE_FUNCTION;
                        Subscriber.remove(ec);
                    }

                    /*! \brief  unannounce an event channel and deregister the event
                     *          channel from the local data structure for publishers
                     *
                     *  \param[in]  ec the unannounced event channel
                     */
                    void unannounce(EC &ec) {
                        TRACE_FUNCTION;
                        Publisher.remove(ec);
                    }

                    /*! \brief  publishes an event from an event channel locally to other
                     *          event channels that have subscribed to the same subject. Thus
                     *          this method can be seen as the local event channel handler.
                     *
                     *  \param[in]  e the event that is published
                     *
                     */
                    void publish_local(const Event &e) {
                        TRACE_FUNCTION;
                        // give start of the SubsriberList
                        typedef famouso::mw::api::SubscriberEventChannel<EL> ec_t;
                        ec_t* sec = static_cast<ec_t*>(Subscriber.select());
                        // traverse the list and call the respective subscriber notify callback
                        // in case subject matching
                        while (sec) {
                            if (sec->subject() == e.subject)
                                sec->callback(e);
                            sec = static_cast<ec_t*>(sec->select());
                        }
                    }

                    /*! \brief Fetches an event from a specific sub network and
                     *         publishes it locally.
                     *
                     *  \param[in]  bnl the network from where an event fetching was requested.
                     */
                    void fetch(famouso::mw::nl::DistinctNL *bnl = 0) {
                        TRACE_FUNCTION;

                        // give start of the SubsriberList
                        typedef famouso::mw::api::SubscriberEventChannel<EL> ec_t;
                        ec_t* sec = static_cast<ec_t*>(Subscriber.select());

                        // traverse the list and try to find the correct subject,
                        // corresponding to the arised event.
                        while (sec) {
                            if (try_fetch_and_process(sec->subject(), sec->snn(), bnl))
                                break;
                            sec = static_cast<ec_t*>(sec->select());
                        }
                    }

                protected:
                    /*! \brief Tries to fetch an event of a given SNN from a specific
                     *         sub network. In case of success it is published locally.
                     *
                     *  \param[in]  subj the subject of the channel
                     *  \param[in]  snn the SNN of the channel
                     *  \param[in]  bnl the network from where an event fetching was requested.
                     */
                    bool try_fetch_and_process(const Subject &subj, const SNN &snn, famouso::mw::nl::DistinctNL *bnl = 0) {
                        Event e(subj);

                        // try to fetch an event from a specific subnet and SNN
                        int8_t result = LL::read(snn, e, bnl);

                        // incoming packet on this SNN?
                        if (result >= 0) {
                            // subject of last incoming packet matches that of the
                            // current event channel

                            // if we got a new event, publish it locally to the
                            // respective subscribers (if the packet was a fragment
                            // the corresponding event may not be complete)
                            if (result > 0)
                                static_cast<EL*>(this)->publish_local(e);

                            // packet/event processed (fetch called for each packet)
                            return true;
                        } else {
                            return false;
                        }
                    }
            };

        } // namespace el
    } // namespace mw
} // namespace famouso

#endif // __EVENTDISPATCHER_H_284D09B9BACF24__

