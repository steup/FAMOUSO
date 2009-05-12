/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
#include "mw/nl/BaseNL.h"
#include "mw/el/EventLayerCallBack.h"

#include "object/Queue.h"

namespace famouso {
    namespace mw {
        namespace el {

            template < class LL >
            class EventLayer : public LL {

                    // potentiell gefaerdete Datenstrukturen
                    // wegen Nebenlaeufigkeit
                    Queue Publisher;
                    Queue Subscriber;

                    void* operator new(size_t, void* __p) {
                        return __p;
                    }

                public:
                    void init() {
                        // late/defered Constructor call on pre-allocated memory
                        // using the placement new of C++
                        new(this) EventLayer;
                        // initialisierungscode
                        // z.B. Callbacks setzen und in allen
                        // Netzen registrieren, CCP in unteren Layer, etc.
                        IncommingEventFromNL.bind<EventLayer<LL>, &EventLayer<LL>::fetch >(this);
                        LL::init();
                    }

                    typedef typename LL::SNN SNN;

                    void announce(famouso::mw::api::EventChannel<EventLayer> &ec) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                        LL::announce(ec.subject(), ec.snn());
                        Publisher.append(ec);
                    }

                    void publish(const famouso::mw::api::EventChannel<EventLayer> &ec, const Event &e) {
                        DEBUG(("%s %p\n", __PRETTY_FUNCTION__, ec.select()));
                        // Hier koennte der Test erfolgen, ob die Subjects vom Event
                        // und vom EventChannel gleich sind, weil nur dann
                        // das Event in diesen Kanal gehoert
                        //
                        // Ist mit einem assert zu machen und auch ob das Subject des
                        // Kanals ueberhaupt gebunden ist
                        //
                        // aber diese Test koennten auch direkt im PublisherEventChannel gemacht werden
                        LL::publish(ec.snn(), e);

                        publish_local(ec, e);
                    }

                    void subscribe(famouso::mw::api::EventChannel<EventLayer> &ec) {
                        DEBUG(("%s %p\n", __PRETTY_FUNCTION__, ec.select()));
                        Subscriber.append(ec);
                        LL::subscribe(ec.subject(), ec.snn());
                    }

                    void unsubscribe(famouso::mw::api::EventChannel<EventLayer> &ec) {
                        Subscriber.remove(ec);
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                    }

                    void unannounce(famouso::mw::api::EventChannel<EventLayer> &ec) {
                        Publisher.remove(ec);
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                    }

                private:

                    // hier erfolgt die local Abarbeitung des Kanaele
                    // eigentlich der LocalEventChannelHandler
                    void publish_local(const famouso::mw::api::EventChannel<EventLayer> &ec, const Event &e) {

                        typedef famouso::mw::api::SubscriberEventChannel< EventLayer >ec_t;
                        ec_t* sec = static_cast<ec_t*>(Subscriber.select());
                        while (sec) {
                            DEBUG(("%s %p %lld %lld\n", __PRETTY_FUNCTION__, sec, sec->subject().value, ec.subject().value));
                            if (sec->subject() == ec.subject())
                                sec->callback(e);
                            sec = static_cast<ec_t*>(sec->select());
                        }
                    }

                public:

                    /*! \brief Fetches an event from a specific sub network.
                     *
                     *  \param[in]  bnl the network from where an event fetching was requested.
                     *
                     *  \todo       The default value of zero describes that we have no fetching
                     *              request from a lower layer. This is for further enhancements
                     *              like fetching in a time-triggered system, where we fetch
                     *              periodically without interrupt support from lower network
                     *              layer.
                     */
                    void fetch(famouso::mw::nl::BaseNL *bnl = 0) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));

                        // give start of the SubsriberList
                        typedef famouso::mw::api::SubscriberEventChannel< EventLayer >ec_t;
                        ec_t* sec = static_cast<ec_t*>(Subscriber.select());

                        // inform low layer about fetching starts
                        LL::event_process_request(bnl);

                        // traverse the list and try to find the correct subject,
                        // corresponding to the arised event.
                        while (sec) {
                            Event e(sec->subject());
                            // fetches an event from a specific subnet
                            if (LL::fetch(sec->snn(), e, bnl)) {
                                // if event was fetched, meaning we have a matching
                                // of subjects, publish it locally to the respective
                                // subscribers and then we are done.
                                publish_local(*sec, e);
                                return;
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

