#include <iostream>
#include "mw/nl/AWDSNL.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/common/Event.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "mw/common/UID.h"
#include "famouso.h"

typedef famouso::mw::nl::AWDSNL<> nl;
typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
typedef famouso::mw::el::EventLayer< anl > el;
typedef famouso::mw::api::EventChannel< el > EC;
typedef famouso::mw::api::PublisherEventChannel<el> PEC;
typedef famouso::mw::api::SubscriberEventChannel<el> SEC;

void cb(famouso::mw::api::SECCallBackData& cbd) {
  std::cout<< "Michaels CallBack " <<  __PRETTY_FUNCTION__
           << "Parameter=" << cbd.length << "Daten="
           << cbd.data << std::endl;
}

int main(int argc, char **argv){

    famouso::init<EC>();
    SEC sec((uint8_t)0xf1);
    sec.subscribe();
    sec.callback.bind<&cb>();

    PEC pec((uint8_t)0xf1);
    pec.announce();
    famouso::mw::Event  e(pec.subject());
    e.length=7;
    e.data=(uint8_t*)"Hallo\r\n";
    pec.publish(e);

    famouso::ios::instance().run();
    return 0;
}
