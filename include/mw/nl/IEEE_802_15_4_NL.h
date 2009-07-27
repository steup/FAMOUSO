/*******************************************************************************
 *
 * Copyright (c) 2008, 2009
 * Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 * Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
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

#ifndef __IEEE_802_15_4_NL_h__
#define __IEEE_802_15_4_NL_h__

#include <stdio.h>

#include "mw/api/EventChannel.h"
#include "mw/common/NodeID.h"
#include "mw/common/Subject.h"
#include "mw/el/EventLayerCallBack.h"
#include "mw/nl/BaseNL.h"
#include "mw/nl/Packet.h"

#include "debug.h"

namespace famouso {
    namespace mw {
        namespace nl {

            /*! \class NL802_15_4
             *  \brief Network layer that acts as the interface to any IEEE 802.15.4 compliant network.
             *
             *  This allows to use IEEE 802.15.4 as a Network Layer within the FAMOUSO
             *  middleware. Depending on the configuration different drivers can be used.
             *  There are two types of drivers available. Driver that use the serial port to
             *  transfer messages to devices that send those message via an IEEE 802.15.4
             *  connection and driver that implement the IEEE 802.15.4 connection themselfe.
             *
             *  \param[in]  driver_t  generic driver for this network layer
             *
             *  \todo use template parameter (enum) to determin actually usable payload size during compile time
             *  \todo test possibility to reduce memory space by using the provided mob_t object
             */
            template< class driver_t >
            class IEEE_802_15_4_NL : public BaseNL {
                    driver_t driver;
                public:
                    typedef IEEE_802_15_4_NL type;
                    typedef famouso::mw::Subject SNN;
                    typedef Packet<SNN> Packet_t;
                    typedef typename driver_t::mob_t mob_t;

                    /*! \brief contains information about the networ layer including maximal available payload */
                    struct info {
                        enum {
                            payload  = 127 - sizeof( SNN )
                        };
                    };

                protected:
                    struct message {
                        uint8_t size;
                        uint8_t data[info::payload];
                    } mbuffer;

                public:

                    IEEE_802_15_4_NL() {}
                    ~IEEE_802_15_4_NL() {}
                    /*! \brief  Initialises the network layer.
                     *
                     *  During this method the parameter of the network layer and the methods
                     *  for interrupt handling are initialized (see delegates for further
                     *  references).
                     */
                    void init() {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                        driver.init();
                        driver.rx_Interrupt.template bind<type, &type::rx_interrupt>(this);
                    }

                    /*! \brief This method binds a subject to a specific network name.
                     *
                     *  \param[in]  s   Subject of the channel
                     *  \param[out] snn Short Network Name
                     */
                    void bind(const Subject &s, SNN &snn) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                        snn = s;
                    }

                    /*! \brief  Publishes messages that are within the available payload of the driver used.
                     *
                     *  \param[in] p  packet to be published
                     */
                    void deliver(const Packet_t& p) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                        struct message buffer;

                        SNN *subject = reinterpret_cast<SNN*>( buffer.data );//TODO Problem mit kleinem subject
                        *subject     = p.snn;

                        buffer.size = 0;
                        while ( buffer.size != p.data_length ) {
                            buffer.data[sizeof( SNN ) + buffer.size] = p.data[buffer.size];
                            ++buffer.size;
                        }
                        buffer.size += sizeof( SNN );
                        driver.send(buffer.data, buffer.size);
                    }

                    /*! \brief  Publishes a fragment of a message that exceeds the available payload.
                     *
                     *  \param[in] p  packet of the fragment to be published
                     */
                    void deliver_fragment(const Packet_t& p) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                    }

                    /*! \brief  Transfers the data of the last received message into a packet
                     *          for further use.
                     *
                     *  \param[out] p   packet that is going to contain the received data
                     */
                    void fetch(Packet_t& p) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));

                        p.snn         = *reinterpret_cast<SNN*>( mbuffer.data );
                        p.data        = mbuffer.data + sizeof( famouso::mw::Subject );
                        p.data_length = mbuffer.size - sizeof( famouso::mw::Subject );
                    }

                    /*! \brief  The rx_interrupt is called from the driver-level as reaction to
                     *          a physical interrupt that was triggered by the arival of a new
                     *          message.
                     */
                    void rx_interrupt(const mob_t& msg) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                        mbuffer.size = 0;
                        while ( mbuffer.size != msg.length) {
                            mbuffer.data[mbuffer.size] = msg.data[mbuffer.size];
                            mbuffer.size++;
                        }
                        famouso::mw::el::IncommingEventFromNL(this);
                    }

                    /*! \brief  The tx_interrupt is called if the driver is able to send
                     *          new/further messages. (not yet implemented)
                     *
                     *  \todo   Provide functionality to use tx_interrupt
                     */
                    void tx_interrupt() {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                    }

                    /*! \brief  Returns Short Network Name of the last received message.
                     *
                     *  \return Short Network Name of the last received message.
                     */
                    SNN lastPacketSNN() {
                        return *reinterpret_cast<SNN*>( mbuffer.data );
                    }
            };
        } // namespace nl
    } // namespace mw
} //namespace famouso

#endif /*__IEEE_802_15_4_NL_h__*/
