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
                        DEBUG(("%s %p\n", __PRETTY_FUNCTION__, this));

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
                        DEBUG(("%s %p\n", __PRETTY_FUNCTION__, this));

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
