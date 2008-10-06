#ifndef __CANNL_h__
#define __CANdNL_h__

#include "mw/nl/BaseNL.h"
#include "mw/nl/Packet.h"
#include "mw/common/NodeID.h"
#include "mw/common/Subject.h"
//#include "mw/api/EventChannel.h"
#include "mw/el/EventLayerCallBack.h"
#include "debug.h"
#include <stdio.h>

namespace famouso {
	namespace mw {
		namespace nl {
/*! \file CANNL.h
 *
 * \class CANNL
 *
 * \brief CAN network layer acts as the interface to the Controller Area Network
 *
 *  The CANNL allows for using the CAN as a Network Layer within the FAMOUSO
 *  middleware. It can use different CAN drivers like SocketCAN, canary from AVR
 *  and so on. The CAN-Driver has to fullfill a certain interface. Furthermore
 *  the CAN Configuration Protocol is used to ensure uniquenesss of CAN IDs.
 *
 */
template< class CAN_Driver >
class CANNL : public BaseNL {
	uint16_t tx_node;
	CAN_Driver driver;
public:
    struct info{
	enum {
	    payload=8
	};
    };

    typedef uint16_t SNN;

    typedef Packet<SNN> Packet_t;


    CANNL() {}
    ~CANNL() {}

    void init(const NodeID &i) {
		// hier muss dass CAN Configuration Protokoll durchlaufen werden.
		// vorher darf man aus dieser Funktion nicht zurueck kommen
		DEBUG(("%s Configuration 64Bit NodeID=%lld\n", __PRETTY_FUNCTION__, i.value));
    }

    // bind Subject to specific networks name
    void bind(const Subject &s, SNN &snn) {
		DEBUG(("%s\n", __PRETTY_FUNCTION__));
		snn=0x1;
    }

    void deliver(const Packet_t& p) {
		// senden des CAN Paketes
		DEBUG(("%s\n", __PRETTY_FUNCTION__));
//		CAN_Driver::MOB m;
//		m.id=genID(NONERT,tx_node,p.snn);
//		m.len=p.len;
//		m.data=p.data;
//		driver.send(&m);
    }

    void deliver_fragment(const Packet_t& p) {
		// Fragmentierung wird zur Zeit noch nicht unterstuetzt
		DEBUG(("%s\n", __PRETTY_FUNCTION__));
    }

    void fetch(Packet_t& p) {
		// ist das Auslesen eines einzelnen Paketes
		DEBUG(("%s\n", __PRETTY_FUNCTION__));
//		driver::MOB mob;
//		driver.receive_blocking(mob);
    }


	void tx_interrupt() {
		DEBUG(("%s\n", __PRETTY_FUNCTION__));
	}

    void rx_interrupt() {
		DEBUG(("%s\n", __PRETTY_FUNCTION__));
		famouso::mw::el::IncommingEventFromNL(this);
    }

};
		} // namespace nl
	} // namespace mw
} //namespace famouso

#endif /* __CANNL_h__ */

