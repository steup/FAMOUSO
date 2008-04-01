#ifndef __AbstractNetworkLayer_h__
#define __AbstractNetworkLayer_h__

#include "debug.h"

// einfacher ANL
template < class NL >
class AbstractNetworkLayer : public NL{
 public:

  typedef typename NL::SNN SNN;

  void announce(const Subject &s, SNN &snn) {
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    NL::bind(s, snn);
    // nach dem Bind noch bekannt geben,
    // das dieser Kanal publiziert wird
  }

  void publish(const SNN &snn, const Event &e) {
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    typename NL::Packet p={snn, &e[0]};
    if (e.getLen() <= NL::info::payload)
      NL::deliver(p);
  }

  void subscribe(const Subject &s, SNN &snn) {
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    NL::bind(s, snn);
    // nach dem Bind auch noch bekannt geben,
    // dass dieser Kanal subscribiert wird
  }

};

#endif /* __AbstractNetworkLayer_h__ */
