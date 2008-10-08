#ifndef __AbstractNetworkLayer_h__
#define __AbstractNetworkLayer_h__

#include "debug.h"

namespace famouso {
	namespace mw {
		namespace anl {

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
  }

};

		} // namespace anl
	} // namespace mw
} //namespace famouso

#endif /* __AbstractNetworkLayer_h__ */
