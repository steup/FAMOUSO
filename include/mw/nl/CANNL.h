#ifndef __CANNL_h__
#define __CANdNL_h__

#include "case/Delegate.h"
#include "mw/nl/BaseNL.h"
#include "mw/nl/Packet.h"
#include "mw/common/NodeID.h"
#include "mw/common/Subject.h"
#include "mw/common/Event.h"
#include "mw/el/EventLayerCallBack.h"
#include "mw/nl/can/canID.h"
#include "debug.h"
#include <stdio.h>
#include "devices/nic/can/peak/PeakCAN.h"

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
template< class CAN_Driver, class CCP, class BP >
class CANNL : public BaseNL {
	uint16_t tx_node;
	CAN_Driver driver;
    CCP ccp;
    BP  etagBP;
	typename CAN_Driver::MOB mob;
public:
    struct info{
        enum {
            payload=8
        };
    };

    typedef uint16_t SNN;

    typedef Packet<SNN> Packet_t;
    Packet_t packet;

    CANNL() : tx_node(0) {}
    ~CANNL() {}

    void init() { init(0);}

    void init(const NodeID &i) {
		// hier muss das CAN Configuration Protokoll durchlaufen werden.
		// vorher darf man aus dieser Funktion nicht zurueck kommen
		DEBUG(("%s Configuration 64Bit NodeID=%lld\n", __PRETTY_FUNCTION__, i.value));
        driver.init();
        tx_node=ccp.ccp_configure_tx_node("Schulze\0", driver);
        famouso::util::Delegate<> dg;
        dg.bind<CANNL<CAN_Driver,CCP,BP>,&CANNL<CAN_Driver,CCP,BP>::rx_interrupt>(this);
        driver.set_rx_Interrupt(dg);
        //driver.interrupts_on(); sowas muss irgendwie noch mit rein
    }

    // bind Subject to specific networks name
    void bind(const Subject &s, SNN &snn) {
//        std::cout<< __PRETTY_FUNCTION__ << std::endl;
		DEBUG(("%s\n", __PRETTY_FUNCTION__));
        /*! \todo binding integrieren */
		snn=etagBP.bind_subject(s,tx_node,driver);
    }

    void deliver(const Packet_t& p) {
		// senden des CAN Paketes
		DEBUG(("%s\n", __PRETTY_FUNCTION__));
		typename CAN_Driver::MOB m;

        m.extended();
        /*! \todo genID function bauen, die vielleicht dann einige Setzungen auf dem AVR vermeidet */
		m.id().prio(0xfd);
		m.id().tx_node(tx_node);
		m.id().etag(p.snn);
		m.len()=0;
        while (m.len() != p.data_length){
            m.data()[m.len()]=p.data[m.len()];
            ++m.len();
        }
		driver.send(m);
    }

    void deliver_fragment(const Packet_t& p) {
		// Fragmentierung wird zur Zeit noch nicht unterstuetzt
		DEBUG(("%s\n", __PRETTY_FUNCTION__));
    }

    void fetch(Packet_t& p) {
		// ist das Auslesen eines einzelnen Paketes
		DEBUG(("%s\n", __PRETTY_FUNCTION__));
        p.data=mob.data();
        p.data_length=mob.len();
    }


	void tx_interrupt() {
		DEBUG(("%s\n", __PRETTY_FUNCTION__));
	}

    /*! \brief This function is called from the driver-level as
     *         reaction to a pysical interrupt that was triggered
     *         by the arriving of a CAN message.
     *
     *  \todo ueber das Auslesen einer CAN Nachricht und deren
     *        Zwischenspeichern muss noch mal nachgedacht werden.
     *        Insbesonder fuer den AVR ist dies vielleicht nicht
     *        immer notwendig bzw der zusaetzliche Speicherbedarf
     *        nicht gerechtfertigt.
     *
     * \todo die Code-Introduktions der verschiedenen Protokolle
     *       sind ueber Aspecte oder ueber Feature-Orientierte
     *       Technicken zu realisieren.
     */
    void rx_interrupt() {
		DEBUG(("%s\n", __PRETTY_FUNCTION__));
		driver.receive(&mob);
        if ( ccp.handle_ccp_configure_request(mob, driver) )
            return;
        if ( etagBP.handle_subject_bind_request(mob, driver) )
            return;
		famouso::mw::el::IncommingEventFromNL(this);
    }

    SNN lastPacketSNN() {
        return mob.id().etag();
    }
};
		} // namespace nl
	} // namespace mw
} //namespace famouso

#endif /* __CANNL_h__ */

