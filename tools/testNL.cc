#include <stdio.h>
#include "mw/nl/CANNL.h"
#include "mw/nl/voidNL.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/common/Event.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "devices/nic/can/peak/PeakCAN.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/nl/can/canID.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"
#include "mw/common/UID.h"
#include "famouso.h"

char *dev = "/dev/pcan33";

typedef device::nic::CAN::PeakCAN<dev, 0x001c> can;
typedef famouso::mw::nl::CAN::detail::ID ID;
typedef can::MOB	mob;

typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> nl;
typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
typedef famouso::mw::el::EventLayer< anl > el;
typedef famouso::mw::api::EventChannel< el > EC;
typedef famouso::mw::api::PublisherEventChannel<el> PEC;
typedef famouso::mw::api::SubscriberEventChannel<el> SEC;


// void cb(const EventChannel<EL>& m){
//   printf("%s Parameter=%d\n", __PRETTY_FUNCTION__, m.snn());
// }

void cb(famouso::mw::api::SECCallBackData& cbd) {
  printf("Michaels CallBack %s Parameter=%d Daten:=%s\n", __PRETTY_FUNCTION__, cbd.length, cbd.data);
}



int main(int argc, char **argv){

  famouso::init<EC>();
  SEC sec((uint8_t)0xf1);
  sec.subscribe();
  sec.callback.bind<&cb>();

  pause();
  return 0;
}
