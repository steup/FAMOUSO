#ifndef __AbstractNetworkLayer_h__
#define __AbstractNetworkLayer_h__

#include "debug.h"
#include "mw/common/Event.h"

namespace famouso {
	namespace mw {
		namespace anl {

// einfacher ANL
template < class NL >
class AbstractNetworkLayer : public NL{
 public:

  typedef typename NL::SNN SNN;
  SNN   subscribe_SNN;

  void announce(const Subject &s, SNN &snn) {
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    NL::bind(s, snn);
    // nach dem Bind noch bekannt geben,
    // das dieser Kanal publiziert wird
  }

  void publish(const SNN &snn, const Event &e) {
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    typename NL::Packet_t p(snn, &e[0], e.length);
    if (e.length <= NL::info::payload)
      NL::deliver(p);
    else
      DEBUG(("Event is to big to deliver at once and fragmentation is not supported at the moment\n"));
  }

  void subscribe(const Subject &s, SNN &snn) {
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    NL::bind(s, snn);
    // nach dem Bind auch noch bekannt geben,
    // dass dieser Kanal subscribiert wird
    typename NL::Packet_t p(subscribe_SNN, const_cast<uint8_t*>(&s.tab[0]), 8);
    NL::deliver(p);
  }

//    bool retrieveSubject(const Subject &s, SNN &snn) {

  bool fetch(const SNN &snn, famouso::mw::nl::BaseNL *bnl) {
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    if ( snn == NL::lastPacketSNN() ) {
        return true;
    } else {
        return false;
    }
  }

    bool getEvent(Event &e){
        typename NL::Packet_t p;
        NL::fetch(p);
        e.length=p.data_length;
        e.data=p.data;
        return true;
    }

    // vorgesehen, um anzuzeigen, dass das Event vollstaendig
    // propagiert wurde und die moeglicherweise reservierten
    // Ressouren wiederverwendet werden koennen
    void event_processed() {}

    void init() {
        NL::init();
        Subject s("SUBSCRIBE");
        NL::bind(s,subscribe_SNN);
    }

};

		} // namespace anl
	} // namespace mw
} //namespace famouso

#endif /* __AbstractNetworkLayer_h__ */
