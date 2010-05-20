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

#ifndef __Gateway_h__
#define __Gateway_h__

#include "debug.h"
#include "mw/nl/DistinctNL.h"
#include "mw/el/EventLayer.h"
#include "mw/api/SubscriberEventChannel.h"
#include "mw/gwl/GatewayEventChannel.h"
#include "mw/common/Event.h"
#include "object/Storage.h"

namespace famouso {
    namespace mw {
        namespace gwl {

            /*! \brief The gateway layer implements the functionality of a gateway
             *
             *  The gateway functionality contains:
             *      -# recognizing of subscriptions and establishing of a proxy for that subscription
             *      -# selectively forwarding (an event should not be reflected to the same network)
             *      -# managing resources
             *      -# only on proxy per subject on a network
             *
             * \tparam ECH is a configured famouso::mw::el::EventLayer
             * \tparam Storage is the depot where the generated forwarding channels
             *         are stored/managed. The default parameter is object::Storage
             */
            template < class ECH, template < typename > class Storage=object::Storage >
            class Gateway  : private famouso::mw::api::SubscriberEventChannel<ECH> {
                    typedef GatewayEventChannel<ECH> GEC;
                    typedef typename Storage<GEC>::depot depot;

                    depot gecs;
                public:

                    /*! \brief Initalizes the gateway and activates a subscription channel.
                     */
                    Gateway() : famouso::mw::api::SubscriberEventChannel< ECH > (famouso::mw::Subject("SUBSCRIBE")) {
                        this->subscribe();
                        this->callback.template bind<Gateway, &Gateway::subscribe_from_network>(this);
                    }

                private:

                    void subscribe_from_network(famouso::mw::api::SECCallBackData& cbd) {
                        if (this->ech().get_network_id() == 0) {
                            ::logging::log::emit() << "Lokal Subscription" << ::logging::log::endl;
                        } else {
                            if (this->subject() == famouso::mw::Subject(cbd.data)) {
                                ::logging::log::emit() << "Subscribe Message of another gateway"
                                    << ::logging::log::endl;
                            } else {
                                typename depot::iterator ii = gecs.begin();
                                while (ii != gecs.end()) {
                                    /*! todo base network has to be checked */
                                    if ((*ii).subject() == famouso::mw::Subject(cbd.data)) {
                                        ::logging::log::emit() << "forward channel exits"
                                            << ::logging::log::endl;
                                        return;
                                    }
                                    ii++;
                                }
                                // hier muss ein neuer rein
//                                std::string str(reinterpret_cast<const char*>(cbd.data), 0, 8);
                                GEC *p=gecs.newElement();
                                if ( p ) {
                                    ::logging::log::emit()
                                        << "Generate a new proxy channel for forwarding events of Subject "
                                        << cbd.data << ::logging::log::endl;
                                    new(p) GEC(famouso::mw::Subject(cbd.data), this->ech().get_network_id());
                                }
                            }
                        }
                    }
            };
        } // namespace gwl
    } // namespace mw
} //namespace famouso

#endif /* __Gateway_h__ */
