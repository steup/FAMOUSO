//#define TEST_ALL
#ifdef __AVR__
#define MAX_LEN 0xff
#else
#define MAX_LEN 0xffff
#endif

#ifdef SECOND_SUBJECT
#define SUBJECT "SUBJECT2"
#else
#define SUBJECT "SUBJECT1"
#endif

#define CPU_FREQUENCY 16000000

#include "test_gen.h"
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




uint8_t buffer[MAX_LEN];

void publish(famouso::config::PEC & pec, famouso::mw::Event & event, uint32_t & counter) {
    create_event_content(event.length, event.data);
    FAMOUSO_ASSERT(check_event_content(event.length, event.data));
    pec.publish(event);
#ifndef __AVR__
    usleep(10000);
#endif
    counter++;
}


int main() {
    famouso::init<famouso::config>();

    famouso::config::PEC pec(SUBJECT);
    pec.announce();

    famouso::mw::Event event(pec.subject());
    event.data = buffer;
    uint32_t counter = 0;

#ifdef TEST_ALL
    for (event.length = 1; event.length < MAX_LEN; event.length++) {
        publish(pec, event, counter);
    }
    publish(pec, event, counter);
#else
    uint16_t step = MAX_LEN / 2000;
    if (!step) step = 1;
    for (event.length = 1; event.length < MAX_LEN / 100; event.length += step) {
        publish(pec, event, counter);
    }

    step = MAX_LEN / 2000;
    if (!step) step = 1;
    for (event.length = MAX_LEN / 100 + 1; event.length < MAX_LEN / 10; event.length += step) {
        publish(pec, event, counter);
    }

    step = MAX_LEN / 200;
    if (!step) step = 1;
    for (event.length = MAX_LEN / 10 + 1; event.length < MAX_LEN - 2*step; event.length += step) {
        publish(pec, event, counter);
    }

    event.length = MAX_LEN; {
        publish(pec, event, counter);
    }
#endif

    ::logging::log::emit() << SUBJECT << ": Published events: " << counter << ::logging::log::endl;

    return 0;
}

