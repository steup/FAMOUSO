#include "famouso.h"

#ifdef __AVR__

#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"
#include "mw/nl/CANNL.h"
#include "devices/nic/can/at90can/avrCANARY.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"
#include "mw/afp/Config.h"

struct MinAFPConfig : famouso::mw::afp::DefaultConfig {
    typedef object::OneBlockAllocator<famouso::mw::afp::EmptyType, EVENT_LENGTH> Allocator;
#if (EVENT_LENGTH <= 255)
    typedef famouso::mw::afp::MinimalSizeProp SizeProperties;
#else
    typedef famouso::mw::afp::DefaultEventSizeProp SizeProperties;
#endif
    enum {
        overflow_error_checking = false
    };
};

namespace famouso {
    namespace CAN {
        class config {

                typedef device::nic::CAN::avrCANARY can;
                typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
                typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
                typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> nl;
#if defined(AFP_ANL_CONFIG_DEF)
                AFP_ANL_CONFIG_DEF
                typedef famouso::mw::anl::AbstractNetworkLayer< nl, AFP_ANL_CONFIG_FRAG, AFP_ANL_CONFIG_DEFRAG > anl;
#else
                typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
#endif
            public:
                typedef famouso::mw::el::EventLayer< anl > EL;
                typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
                typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
        };
    }

    typedef CAN::config config;
}
#else

#include "famouso_bindings.h"

#endif

