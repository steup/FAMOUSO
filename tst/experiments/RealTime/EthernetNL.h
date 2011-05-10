/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2011 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __EthernetNL_h__
#define __EthernetNL_h__

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

            /*! \brief Ethernet network layer
             *
             *  \tparam Driver  Driver to use to send and receive packets. Currently,
             *                  only supports XenomaiEthernet.
             */
            template< class Driver >
            class EthernetNL : public DistinctNL {
                    typedef EthernetNL   type;
                    Driver driver;
                    typename Driver::MOB * mob;

                    struct PacketHeader {
                        uint8_t subject [8];
                        uint16_t length;    // Needed because of padding to 60 bytes
                    } __attribute__((packed));

                public:
                    struct info {
                        enum {
                            payload = 1500 - sizeof(PacketHeader)
                        };
                    };

                    typedef famouso::mw::Subject SNN;

                    typedef Packet<SNN> Packet_t;

                    /// Default AFP %configuration of this network layer (see \ref afp_config_nl)
                    typedef afp::MultiSubjectESeqReorderDuplicateConfig<SNN> AFP_Config;
                    /*
                    typedef afp::MultiSubjectConfig<SNN> AFP_Config;
                    */


                    EthernetNL() : mob(0) {}
                    ~EthernetNL() {}

                    /*! \brief  Initialises the network layer.
                     */
                    void init() {
                        TRACE_FUNCTION;
                        driver.init();
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
                        snn = s;
                    }

                    /*! \brief  deliver a packet within using the configured driver
                     *
                     *  \param[in] p  packet to be delivered
                     */
                    void deliver(const Packet_t& p) {
                        TRACE_FUNCTION;
                        FAMOUSO_ASSERT(p.data_length <= info::payload);

                        typename Driver::MOB m;

                        PacketHeader * hdr = reinterpret_cast<PacketHeader *>(m.data());
                        memcpy(hdr->subject, p.snn.tab(), 8);
                        hdr->length = htons((p.fragment ? 0x8000 : 0) | p.data_length);

                        memcpy(m.data() + sizeof(PacketHeader), p.data, p.data_length);
                        m.len() = p.data_length + sizeof(PacketHeader);

                        driver.transmit(m);
                    }

                    /*! \brief  takes the last received network packet message
                     *          and save it into a packet
                     *
                     *  \param[out] p   packet that is filled with the received data
                     */
                    void take(Packet_t& p) {
                        TRACE_FUNCTION;
                        FAMOUSO_ASSERT(mob);
                        PacketHeader * hdr = reinterpret_cast<PacketHeader *>(mob->data());
                        p.data_length = ntohs(hdr->length);
                        p.fragment = p.data_length & 0x8000;
                        p.data_length &= ~0x8000;
                        FAMOUSO_ASSERT(mob->len() >= p.data_length + sizeof(PacketHeader));
                        p.data = mob->data() + sizeof(PacketHeader);
                    }


                    /*! \brief This function is called from the driver-level when
                    *          a packet arrived.
                     */
                    void rx_interrupt() {
                        TRACE_FUNCTION;
                        mob = driver.receive();
                        famouso::mw::el::IncommingEventFromNL(this);
                    }

                    /*! \brief  Returns the Short Network Name of the last received message.
                     *
                     *  \return Short Network Name of the last received message.
                     */
                    SNN lastPacketSNN() {
                        FAMOUSO_ASSERT(mob);
                        PacketHeader * hdr = reinterpret_cast<PacketHeader *>(mob->data());
                        return SNN(hdr->subject);
                    }

            };
        } // namespace nl
    } // namespace mw
} //namespace famouso

#endif /* __EthernetNL_h__ */

