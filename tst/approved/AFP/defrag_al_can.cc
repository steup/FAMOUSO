#define CPU_FREQUENCY 16000000

#ifdef AFP_CLASSIC_CONFIG
#include "mw/afp/DefragConfigC.h"
#else
#include "mw/afp/DefragConfig.h"
#endif
#include "mw/afp/AFPSubscriberEventChannel.h"

#include "famouso.h"
#include "util/Idler.h"


#ifdef __AVR__

#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"
#include "mw/nl/CANNL.h"
#include "devices/nic/can/at90can/avrCANARY.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"

namespace famouso {
    namespace CAN {
        class config {

                typedef device::nic::CAN::avrCANARY can;
                typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
                typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
                typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> nl;
                typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
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


using namespace famouso::mw;

#ifdef AFP_CLASSIC_CONFIG
typedef afp::DefragConfigC <
    afp::OneSubject,
    afp::NoEventSeqSupport,
    afp::NoDuplicateChecking,
    afp::NoReorderingSupport,
    afp::NoFECSupport,
    afp::MinimalSizeProp
> MinDefragConfig;
#else
typedef afp::DefragConfig <
    afp::one_subject | afp::ideal_network |
    afp::max_event_length_255 | afp::no_overflow_error_checking
> MinDefragConfig;
#endif



void ReceiveCallback(famouso::mw::api::SECCallBackData& cbd) {
    ::logging::log::emit() << cbd.data;
}

int main() {
    enum { mtu = 8 };
    famouso::init<famouso::config>();

    afp::AFPSubscriberEventChannel<famouso::config::SEC, MinDefragConfig, Event> sec("SUBJECT_", mtu);
    sec.callback.bind<ReceiveCallback>();
    sec.subscribe();

    Idler::idle();

    return 0;
}

