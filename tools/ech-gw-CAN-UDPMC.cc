#include <iostream>

// CAN-NL specific includes
#include "devices/nic/can/peak/PeakCAN.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/nl/can/canID.h"
#include "mw/nl/can/etagBP/Broker.h"
#include "mw/nl/can/ccp/Broker.h"
#include "mw/nl/CANNL.h"

// UDPBroadCastNL specific
#include "mw/nl/UDPMultiCastNL.h"

// AWDSNL specific
//#include "mw/nl/AWDSNL.h"
//#include "mw/nl/voidNL.h"

// common famouso middlware includes
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/nal/NetworkAdapter.h"
#include "mw/gwl/Gateway.h"
#include "mw/el/EventLayer.h"
#include "mw/el/EventLayerMiddlewareStub.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"
#include "mw/common/Event.h"

#include "famouso.h"

#include "util/Idler.h"

namespace famouso {

    namespace GW_CAN_UDPMC {
        const char *dev = "/dev/pcan32";
        class config {
                typedef device::nic::CAN::PeakCAN<dev, 0x011c> can;
                typedef famouso::mw::nl::CAN::ccp::Broker<can> ccpBroker;
                typedef famouso::mw::nl::CAN::etagBP::Broker<can> etagBroker;
                typedef famouso::mw::nl::CANNL<can, ccpBroker, etagBroker> nlCAN;
                typedef famouso::mw::anl::AbstractNetworkLayer< nlCAN > anlCAN;

                typedef famouso::mw::nl::UDPMultiCastNL nlUDP;
                typedef famouso::mw::anl::AbstractNetworkLayer< nlUDP > anlUDP;

                typedef famouso::mw::nal::NetworkAdapter< anlCAN, anlUDP > na;
                typedef famouso::mw::el::EventLayer< na > el;
            public:
                typedef famouso::mw::gwl::Gateway<el> GW;
                typedef famouso::mw::el::EventLayerMiddlewareStub< el > ELMS;
                typedef famouso::mw::api::EventChannel< el > EC;
                typedef famouso::mw::api::PublisherEventChannel<el> PEC;
                typedef famouso::mw::api::SubscriberEventChannel<el> SEC;
        };
    }

    template <class config>
    class Configurator : public config {
            typedef typename config::ELMS   localELMS;
            typename config::GW     GW;
        public:
            Configurator() {
                famouso::init<typename config::EC>();
                static localELMS elms;
                GW.start();
            }
    };

    typedef Configurator<GW_CAN_UDPMC::config> config;
}

int main (int argc, char **argv) {
    std::cout << "Project: FAMOUSO" << std::endl;
    std::cout << "local Event Channel Handler" << std::endl << std::endl;
    std::cout << "Author: Michael Schulze" << std::endl;
    std::cout << "Revision: $Rev$" << std::endl;
    std::cout << "$Date$" << std::endl;
    std::cout << "last changed by $Author$" << std::endl << std::endl;
    std::cout << "build Time: " << __TIME__ << std::endl;
    std::cout << "build Date: " << __DATE__ << std::endl << std::endl;

    try {
        famouso::config _famouso;
        std::cout << "FAMOUSO -- Initalisation successfull" << std::endl << std::endl;
        Idler::idle();
        std::cout << "FAMOUSO -- successfully finished" << std::endl;
    } catch (const char *e) {
        std::cerr << "Exception: " << e << std::endl;
    }
    return 0;
}

