#ifndef __voidNL_h__
#define __voidNL_h__

#include "mw/common/NodeID.h"
#include "mw/common/Subject.h"
#include "mw/api/EventChannel.h"
#include "debug.h"
#include <stdio.h>

class voidNL {
public:
    struct info{
	enum {
	    payload=8
	};
    };

    typedef uint16_t SNN;

    struct Packet {
	const SNN &snn;
	uint8_t	*data;
    };


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

    void deliver(const Packet& p) {
	DEBUG(("%s\n", __PRETTY_FUNCTION__));
    }

    void deliver_fragment(const Packet& p) {
	DEBUG(("%s\n", __PRETTY_FUNCTION__));
    }

    void fetch(Packet& p) {
	DEBUG(("%s\n", __PRETTY_FUNCTION__));
    }

    void interrupt() {
	DEBUG(("%s\n", __PRETTY_FUNCTION__));
    }

};

#endif /* __voidNL_h__ */

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
