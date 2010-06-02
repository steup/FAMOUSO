/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/

#ifndef __CANNL_h__
#define __CANNL_h__

#include "case/Delegate.h"
#include "mw/nl/DistinctNL.h"
#include "mw/nl/Packet.h"
#include "mw/afp/Config.h"
#include "mw/common/NodeID.h"
#include "mw/common/Subject.h"
#include "mw/common/Event.h"
#include "mw/el/EventLayerCallBack.h"
#include "mw/nl/can/canID.h"
#include "debug.h"
#include <stdio.h>

namespace famouso {
    namespace mw {
        namespace nl {

            /*! \brief CAN network layer acts as the interface to the Controller Area Network
             *
             *  The CANNL allows for using the CAN as a Network Layer within the FAMOUSO
             *  middleware. It can use different CAN drivers like SocketCAN, canary from AVR
             *  and so on. The CAN-Driver has to fullfill a certain interface. Furthermore
             *  the CAN Configuration Protocol is used to ensure uniquenesss of CAN IDs.
             *
             */
            template< class CAN_Driver, class CCP, class BP >
            class CANNL : public DistinctNL {
                    typedef CANNL   type;
                    uint16_t tx_node;
                    CAN_Driver driver;
                    CCP ccp;
                    BP  etagBP;
                    typename CAN_Driver::MOB mob;
                public:
                    struct info {
                        enum {
                            payload = 8
                        };
                    };

                    typedef uint16_t SNN;

                    typedef Packet<SNN> Packet_t;

                    /// Default AFP %configuration of this network layer (see \ref afp_config_nl)
                    typedef afp::Disable AFP_Config;


                    CANNL() : tx_node(0) {}
                    ~CANNL() {}

                    /*! \brief  Initialises the CAN network layer.
                     *
                     *  During this method the parameter of the network layer and the methods
                     *  for interrupt handling are initialized (see delegates for further
                     *  references).
                     */
                    void init() {
                        TRACE_FUNCTION;
                        ::logging::log::emit< ::logging::Info>()
                            << " Configuration 64Bit NodeID=" << i.value()
                            << ::logging::log::endl;
                        driver.init();
                        // the CAN Configuration Protocol has to run, before this
                        // function returns
                        tx_node = ccp.ccp_configure_tx_node(UID("Schulze\0"), driver);
                        famouso::util::Delegate<> dg;
                        dg.bind<type, &type::rx_interrupt>(this);
                        driver.set_rx_Interrupt(dg);
                        driver.rx_interrupts(true);
                    }

                    /*! \brief bind a subject to a specific network name.
                     *
                     *  \param[in]  s   Subject of the channel
                     *  \param[out] snn Short Network Name
                     */
                    void bind(const Subject &s, SNN &snn) {
                        TRACE_FUNCTION;
                        snn = etagBP.bind_subject(s, tx_node, driver);
                    }

                    /*! \brief  deliver a packet within using the configured driver
                     *
                     *  \param[in] p  packet to be delivered
                     */
                    void deliver(const Packet_t& p) {
                        // senden des CAN Paketes
                        TRACE_FUNCTION;
                        typename CAN_Driver::MOB m;

                        m.extended();
                        m.id().prio(0xfd);
                        m.id().fragment(p.fragment);
                        m.id().tx_node(tx_node);
                        m.id().etag(p.snn);
                        m.len() = 0;
                        while (m.len() != p.data_length) {
                            m.data()[m.len()] = p.data[m.len()];
                            ++m.len();
                        }
                        driver.transmit(m);
                    }

                    /*! \brief  fetches the last received message into a packet
                     *
                     *  \param[out] p   packet that is filled with the received data
                     */
                    void fetch(Packet_t& p) {
                        // ist das Auslesen eines einzelnen Paketes
                        TRACE_FUNCTION;
                        p.data = mob.data();
                        p.data_length = mob.len();
                        p.fragment=mob.id().fragment();
                    }


                    /*! \brief  The tx_interrupt is called if the driver is able to send
                     *          new/further messages. (not yet implemented)
                     *
                     *  \todo   Provide functionality to use tx_interrupt
                     */
                    void tx_interrupt() {
                        TRACE_FUNCTION;
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
                        TRACE_FUNCTION;
                        driver.receive(&mob);
                        if (ccp.handle_ccp_configure_request(mob, driver))
                            return;
                        if (etagBP.handle_subject_bind_request(mob, driver))
                            return;
                        famouso::mw::el::IncommingEventFromNL(this);
                    }

                    /*! \brief  Returns the Short Network Name of the last received message.
                     *
                     *  \return Short Network Name of the last received message.
                     */
                    SNN lastPacketSNN() {
                        return mob.id().etag();
                    }

            };
        } // namespace nl
    } // namespace mw
} //namespace famouso

#endif /* __CANNL_h__ */

