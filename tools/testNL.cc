#include <stdio.h>
#include "mw/nl/voidNL.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/common/Event.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

typedef famouso::mw::nl::voidNL	nl;
typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
typedef famouso::mw::el::EventLayer< anl > el;
typedef famouso::mw::api::EventChannel< el > EC;
typedef famouso::mw::api::PublisherEventChannel<EC> PEC;
typedef famouso::mw::api::SubscriberEventChannel<EC> SEC;


// void cb(const EventChannel<EL>& m){
//   printf("%s Parameter=%d\n", __PRETTY_FUNCTION__, m.snn());
// }

void cb(EC::CallBackData& cbd) {
  printf("%s Parameter=%d Daten:=%s\n", __PRETTY_FUNCTION__, cbd.length, cbd.data);
}



int main(int argc, char **argv){

  SEC sec((uint8_t)0xf1);
  sec.callback.from_function<EC, &EC::cb>(&sec);
  sec.callback.from_function<&cb>();
  sec.subscribe();
  //  {
  SEC sec1((uint8_t)0xf1);
  sec1.callback.from_function<EC, &EC::cb>(&sec1);
  sec1.subscribe();
  //  }

  printf ("\n");
  PEC ec((uint8_t)0xf1);
  ec.callback.from_function<EC, &EC::cb>(&ec);
  printf ("sizeof(ec)=%d snn=%d\n",sizeof(ec), ec.snn());


//   ec.callback(ec);
  ec.announce();
//   ec.callback(sec);
  printf ("sizeof(ec)=%d snn=%d\n\n",sizeof(ec), ec.snn());

  famouso::mw::Event e(ec.subject());
  ec.publish(e);

  return 0;
}
