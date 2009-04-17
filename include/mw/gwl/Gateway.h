#ifndef __Gateway_h__
#define __Gateway_h__

#include "iostream"
#include "debug.h"
#include "mw/nl/BaseNL.h"
#include "mw/el/EventLayer.h"
#include "mw/api/SubscriberEventChannel.h"
#include "mw/gwl/GatewayEventChannel.h"
#include "mw/common/Event.h"

#include <vector>

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
             *  \tparam ECH is a configured famouso::mw::el::EventLayer
             *
             */
            template < class ECH >
            class Gateway  : private famouso::mw::api::SubscriberEventChannel<ECH> {
                    typedef GatewayEventChannel<ECH> GEC;
                    std::vector<const GEC * >     gecs;

                public:

                    /*! \brief Initalizes the the gateway and activates a subscription channel.
                     */
                    Gateway() : famouso::mw::api::SubscriberEventChannel< ECH > (famouso::mw::Subject("SUBSCRIBE")) {}

                    void start() {
                        this->subscribe();
                        this->callback.template bind<Gateway, &Gateway::subscribe_from_network>(this);
                    }

                    void stop() {
                        this->unsubscribe();
                    }

                private:

                    void subscribe_from_network(famouso::mw::api::SECCallBackData& cbd) {
                        if (this->ech().get_network_id() == 0) {
                            std::cout << "Lokal Subscription" << std::endl;
                        } else {
                            if (this->subject() == famouso::mw::Subject(cbd.data)) {
                                std::cout << "Subscribe Message of another gateway" << std::endl;
                            } else {
                                uint32_t ii = 0;
                                while (ii < gecs.size()) {
                                    // todo base network noch ueberpruefen
                                    if (gecs[ii]->subject() == famouso::mw::Subject(cbd.data)) {
                                        std::cout << "forward channel exits" << std::endl;
                                        return;
                                    }
                                    ii++;
                                }
                                std::string str(reinterpret_cast<const char*>(cbd.data), 0, 8);
                                std::cout << "Generate a new proxy channel for forwarding events of Subject " << str << std::endl;
                                const GEC *g = new GEC(famouso::mw::Subject(cbd.data), this->ech().get_network_id());
                                gecs.push_back(g);
                            }
                        }
                    }
            };
        } // namespace gwl
    } // namespace mw
} //namespace famouso

#endif /* __Gateway_h__ */
