#define CPU_FREQUENCY 16000000

#ifdef AFP_CLASSIC_CONFIG
#include "mw/afp/FragConfigC.h"
#else
#include "mw/afp/FragConfig.h"
#endif
#include "mw/afp/AFPPublisherEventChannel.h"

#include "famouso.h"

#include <stdlib.h>


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
typedef afp::FragConfigC <
    afp::UseNoEventSeq,
    afp::UseNoFEC,
    afp::MinimalSizeProp
> MinFragConfig;
#else
typedef afp::FragConfig <
    afp::one_subject | afp::ideal_network |
    afp::max_event_length_255 | afp::no_overflow_error_checking
> MinFragConfig;
#endif




int main() {
    enum { mtu = 8 };

    famouso::init<famouso::config>();

    afp::AFPPublisherEventChannel<famouso::config::PEC, MinFragConfig, mtu> pec("SUBJECT_");
    pec.announce();

    Event event(pec.subject());

    uint8_t data1 [] = "Hallo Welt!\n";
    uint8_t length1 = sizeof(data1);

    event.data = data1;
    event.length = length1;
    pec.publish(event);

    uint8_t data2 [] = "Das ist ein etwas längerer Testtext...\n";
    uint8_t length2 = sizeof(data2);

    event.data = data2;
    event.length = length2;
    pec.publish(event);

    return 0;
}

