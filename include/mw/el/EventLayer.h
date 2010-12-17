/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
#include "mw/api/SubscriberEventChannel.h"
#include "mw/api/EventChannel.h"
#include "mw/nl/DistinctNL.h"
#include "mw/el/EventLayerCallBack.h"

#include "object/Queue.h"

namespace famouso {
    namespace mw {
        namespace el {

            /*! \brief The event layer provides the main publish/subscribe functionality
             *
             *          It has the ability to get different lower layer plugs and supports
             *          local as well as remote publishing. Furthermore, it handles all
             *          relevant data structure for the management of event channels.
             *
             *  \tparam LL the lower layer is a configurable component see prerequisites.
             *
             *  \pre    The type of template parameters can be an famouso::mw::anl:AbstractNetworkLayer
             *          or an famouso::mw::nal::NetworkAdapter dependent on the configuration of the
             *          middleware stack
             */
            template < class LL >
            class EventLayer : public LL {

                protected:

                    typedef LL LowerLayer;

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

                    /*! \brief  self type
                     */
                    typedef EventLayer type;

                    /*! \brief  initialize the middleware core
                     */
                    void init() {
                        // initialization code
                        // e.g. set callbacks and initalize all lower layers too
                        IncommingEventFromNL.bind<EventLayer<LL>, &EventLayer<LL>::fetch>(this);
                        LL::init();
                    }


                    /*! \brief  announce an event channel on all lower layers and register
                     *          the event channel on a local data structure for publisher
                     *
                     *  \param[in]  ec the publishing event channel that is announced
                     */
                    void announce(famouso::mw::api::EventChannel<EventLayer> &ec) {
                        TRACE_FUNCTION;
                        LL::announce(ec.subject(), ec.snn());
                        Publisher.append(ec);
                    }

                    /*! \brief  publishing within an event channel the given event
                     *
                     *  \param[in]  ec the publishing event channel
                     *  \param[in]  e the event that is published
                     *
                     */
                    void publish(const famouso::mw::api::EventChannel<EventLayer> &ec, const Event &e) {
                        TRACE_FUNCTION;
                        ::logging::log::emit< ::logging::Info>()
                            << PROGMEMSTRING("Publish channel with addr=")
                            << ::logging::log::hex
                            << reinterpret_cast<const void*>(&ec)
                            << PROGMEMSTRING(" with Subject -> [")
                            << ec.subject().value() << ']'
                            << ::logging::log::endl;
                        // publish on all  lower layers/subnets
                        LL::publish(ec.snn(), e);

                        // publish locally on all subscribed event channels
                        publish_local(e);
                    }

                    /*! \brief  subscribe an event channel on all lower layers and register
                     *          the event channel on a local data structure for subscribers
                     *
                     *  \param[in]  ec the subscribing event channel that is announced
                     */
                    void subscribe(famouso::mw::api::EventChannel<EventLayer> &ec) {
                        TRACE_FUNCTION;
                        ::logging::log::emit< ::logging::Info>()
                            << PROGMEMSTRING("Subscribe channel with addr=")
                            << ::logging::log::hex
                            << reinterpret_cast<const void*>(&ec)
                            << PROGMEMSTRING(" with Subject -> [")
                            << ec.subject().value() << ']'
                            << ::logging::log::endl;
                        LL::subscribe(ec.subject(), ec.snn());
                        Subscriber.append(ec);
                    }

                    /*! \brief  unsubscribe an event channel and deregister the event
                     *          channel from the local data structure for subscribers
                     *
                     *  \param[in]  ec the unsubscribed event channel
                     */
                    void unsubscribe(famouso::mw::api::EventChannel<EventLayer> &ec) {
                        TRACE_FUNCTION;
                        Subscriber.remove(ec);
                    }

                    /*! \brief  unannounc an event channel and deregister the event
                     *          channel from the local data structure for publishers
                     *
                     *  \param[in]  ec the unsubscribed event channel
                     */
                    void unannounce(famouso::mw::api::EventChannel<EventLayer> &ec) {
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
                        typedef famouso::mw::api::SubscriberEventChannel<EventLayer> ec_t;
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
                     *
                     *  \todo       The default value of zero describes that we have no fetching
                     *              request from a lower layer. This is for further enhancements
                     *              like fetching in a time-triggered system, where we fetch
                     *              periodically without interrupt support from lower network
                     *              layer.
                     */
                    void fetch(famouso::mw::nl::DistinctNL *bnl = 0) {
                        TRACE_FUNCTION;

                        // give start of the SubsriberList
                        typedef famouso::mw::api::SubscriberEventChannel<EventLayer> ec_t;
                        ec_t* sec = static_cast<ec_t*>(Subscriber.select());

                        // inform low layer about fetching starts
                        LL::event_process_request(bnl);

                        // traverse the list and try to find the correct subject,
                        // corresponding to the arised event.
                        while (sec) {
                            Event e(sec->subject());

                            // try to fetch an event from a specific subnet and SNN
                            int8_t result = LL::fetch(sec->snn(), e, bnl);

                            // incoming packet on this SNN?
                            if (result >= 0) {
                                // subject of last incoming packet matches that of the
                                // current event channel

                                // if we got a new event, publish it locally to the
                                // respective subscribers (if the packet was a fragment
                                // the corresponding event may not be complete)
                                if (result > 0)
                                    publish_local(e);

                                // packet/event processed (fetch called for each packet)
                                break;
                            }
                            sec = static_cast<ec_t*>(sec->select());
                        }
                        // inform lower layer that we are done
                        LL::event_processed();
                    }
            };

        } // namespace el
    } // namespace mw
} // namespace famouso

#endif /* __EventLayer_h__ */

