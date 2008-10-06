#ifndef __voidNL_h__
#define __voidNL_h__

#include "mw/nl/BaseNL.h"
#include "mw/nl/Packet.h"
#include "mw/common/NodeID.h"
#include "mw/common/Subject.h"
#include "mw/api/EventChannel.h"
#include "debug.h"
#include <stdio.h>

namespace famouso {
	namespace mw {
		namespace nl {

class voidNL : public BaseNL {
public:
    struct info{
	enum {
	    payload=8
	};
    };

    typedef uint16_t SNN;

    typedef Packet<SNN> Packet_t;


    voidNL() {}
    ~voidNL() {}

    void init(const NodeID &i) {
	DEBUG(("%s Configuration 64Bit NodeID=%lld\n", __PRETTY_FUNCTION__, i.value));
    }

    // bind Subject to specific networks name
    void bind(const Subject &s, SNN &snn) {
	DEBUG(("%s\n", __PRETTY_FUNCTION__));
	snn=0x1;
    }

    void deliver(const Packet_t& p) {
	DEBUG(("%s\n", __PRETTY_FUNCTION__));
    }

    void deliver_fragment(const Packet_t& p) {
	DEBUG(("%s\n", __PRETTY_FUNCTION__));
    }

    void fetch(Packet_t& p) {
	DEBUG(("%s\n", __PRETTY_FUNCTION__));
    }

    void interrupt() {
	DEBUG(("%s\n", __PRETTY_FUNCTION__));
    }

};
		} // namespace nl
	} // namespace mw
} //namespace famouso

#endif /* __voidNL_h__ */

