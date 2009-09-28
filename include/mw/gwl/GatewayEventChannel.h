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

#ifndef __GatewayEventChannel_h__
#define __GatewayEventChannel_h__

#include "mw/api/SubscriberEventChannel.h"
#include "mw/nl/BaseNL.h"
#include "mw/common/CallBack.h"

namespace famouso {
    namespace mw {
        namespace gwl {

            /*! \class GatewayEventChannel
             *
             *  \brief The GatewayEventChannel is a SubscriberEventChannel with additional
             *         proxy functionality for publishing/forwarding events to the right
             *         destination network.
             *
             *         The most functionality is reused from the base class. Hence, it acts
             *         as a normal SubscriberEventChannel. Addtionally, it provides a special
             *         callback the realise the forwarding funcionality and it knows the network
             *         from where the subscription came.
             *
             *  \tparam ECH needs to be a configured type of famouso::mw::el::EventLayer
             */
            template < class ECH >
            class GatewayEventChannel : public famouso::mw::api::SubscriberEventChannel<ECH> {

                    typedef famouso::mw::api::SubscriberEventChannel< ECH > BASE;

                    const famouso::mw::nl::BaseNL * const _bnl;
                public:

                    /*! \brief Constructs the forwarding channel.
                     *
                     *    Subscribe the event channel and reserve resources
                     *    and call the needed functionalities within the
                     *    sublayers to bind the subject etc.
                     *
                     *    Furthermore establish proxy functionality by binding
                     *    the respective callback for publishing.
                     */
                    GatewayEventChannel(const Subject &s, const famouso::mw::nl::BaseNL * const bnl) : BASE(s), _bnl(bnl) {
                        log::emit< ::logging::Trace>() << FUNCTION_SIGNATURE << log::endl;

                        this->ech().subscribe(*this);
                        // initialize the forwarding callback
                        this->callback.template bind<GatewayEventChannel, &GatewayEventChannel::forward>(this);
                    }

                private:
                    /*! \brief The forward function is a callback.
                     *
                     *    This callback is called by the famouso::mw::el::EventLayer
                     *    when an event according to that channel arises. This allows
                     *    for forwarding of the event to the respective sublayer.
                     *
                     *    \param[in] cbd the event that needs to be forwarded.
                     */
                    void forward(famouso::mw::api::SECCallBackData& cbd) {
                        log::emit< ::logging::Trace>() << FUNCTION_SIGNATURE << log::endl;

                        // If the event is from the another network as the subscription it will
                        // be published, otherwise it would an reflection in the same network
                        // the event would arise twice at the subscribers.
                        if (! this->ech().event_from_network_with_id(_bnl))
                            this->ech().publish_to_network_with_id(this->snn(), cbd, _bnl);
                    }

            };

        } // namespace gwl
    } // namespace mw
} // namespace famouso


#endif /* __GatewayEventChannel_h__ */
