#define CPU_FREQUENCY 16000000

#include "test_gen.h"
#include "famouso.h"
#include "util/Idler.h"
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

// output subjects to logging framework
::logging::loggingReturnType & operator<<(::logging::loggingReturnType & out, const famouso::mw::Subject & s) {
    for (uint8_t c = 0; c < 8; c++)
        out << s.tab()[c];
    return out;
}


void ReceiveCallback(famouso::mw::api::SECCallBackData& cbd) {
    static uint32_t i = 0;
    if (check_event_content(cbd.length, cbd.data)) {
        ::logging::log::emit() << cbd.subject << ": Received correct event #" << (++i) << " (length " << cbd.length << ")" << ::logging::log::endl;
    } else {
        ::logging::log::emit() << cbd.subject << ": RECEIVED INCORRECT EVENT! (length " << cbd.length << ")" << ::logging::log::endl;
        abort();
    }
}



int main() {
    famouso::init<famouso::config>();

    famouso::config::SEC sec1("SUBJECT1");
    sec1.callback.bind<ReceiveCallback>();
    sec1.subscribe();

    famouso::config::SEC sec2("SUBJECT2");
    sec2.callback.bind<ReceiveCallback>();
    sec2.subscribe();

    Idler::idle();

    return 0;
}

