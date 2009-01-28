#include <iostream>

// CAN-NL specific includes
#include "devices/nic/can/peak/PeakCAN.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/nl/can/canID.h"
#include "mw/nl/can/etagBP/Broker.h"
#include "mw/nl/can/ccp/Broker.h"
#include "mw/nl/CANNL.h"

// UDPBroadCastNL specific
#include "mw/nl/UDPBroadCastNL.h"

// common famouso middlware includes
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/el/EventLayerMiddlewareStub.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"
#include "mw/common/Event.h"

#include "famouso.h"

#include "util/Idler.h"

namespace famouso {
    namespace CAN {
        char *dev = "/dev/pcan32";
        class config {
                typedef device::nic::CAN::PeakCAN<dev, 0x011c> can;
                typedef famouso::mw::nl::CAN::ccp::Broker<can> ccpBroker;
                typedef famouso::mw::nl::CAN::etagBP::Broker<can> etagBroker;
                typedef famouso::mw::nl::CANNL<can, ccpBroker, etagBroker> nl;
                typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
                typedef famouso::mw::el::EventLayer< anl > el;
            public:
                typedef famouso::mw::el::EventLayerMiddlewareStub< el > ELMS;
                typedef famouso::mw::api::EventChannel< el > EC;
                typedef famouso::mw::api::PublisherEventChannel<el> PEC;
                typedef famouso::mw::api::SubscriberEventChannel<el> SEC;
        };
    }

    namespace UDP {
        class config {
                typedef famouso::mw::nl::UDPBroadCastNL nl;
                typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
                typedef famouso::mw::el::EventLayer< anl > el;
            public:
                typedef famouso::mw::el::EventLayerMiddlewareStub< el > ELMS;
                typedef famouso::mw::api::EventChannel< el > EC;
                typedef famouso::mw::api::PublisherEventChannel<el> PEC;
                typedef famouso::mw::api::SubscriberEventChannel<el> SEC;
        };
    }

    typedef CAN::config config;
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
        famouso::init<famouso::config::EC>();
        famouso::config::ELMS localELMS;
        std::cout << "FAMOUSO -- Initalisation successfull" << std::endl << std::endl;
        Idler::idle();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    std::cout << "FAMOUSO -- successfully finished" << std::endl;
    return 0;
}

