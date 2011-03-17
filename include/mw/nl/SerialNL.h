/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
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
#ifndef __SERIAL_NL_h__
#define __SERIAL_NL_h__

#include <stdio.h>

#include "mw/afp/Config.h"
#include "mw/api/EventChannel.h"
#include "mw/common/NodeID.h"
#include "mw/common/Subject.h"
#include "mw/el/EventLayerCallBack.h"
#include "mw/nl/DistinctNL.h"
#include "mw/nl/Packet.h"

#include "debug.h"

namespace famouso {
    namespace mw {
        namespace nl {
            /*! \brief  Network Layer for serial communication.
             *
             *  This network layer enables the use of a serial communication
             *  within the %FAMOUSO middleware.
             *
             *  \param[in]  DRV  generic driver for this network layer
             */
            template< class DRV >
            class SerialNL : public DistinctNL {
                public:
                    typedef SerialNL<DRV> type;
                    typedef famouso::mw::Subject SNN;
                    typedef Packet<SNN> Packet_t;
                    /// Default AFP %configuration of this network layer (see \ref afp_config_nl)
                    typedef afp::Disable AFP_Config;
                    typedef typename DRV::mob_t mob_t;

                    /*! \brief information about the network layer*/
                    struct info {
                        enum {
                            payload  = DRV::cfgInfo::payload - sizeof( SNN )
                            /*!< available payload*/
                        };
                    };
                protected:
                    DRV driver;
                    mob_t rmob;
                public:
                    SerialNL()  {}
                    ~SerialNL() {}

                    /*! \brief  Initialises the network layer.
                     *
                     *  If this method is called the %ArMARoW %driver of %FAMOUSO
                     *  is (re)initialized and the methods for interrupt handling
                     *  are bound to the appropriated %driver methods
                     *  (see delegates for further references).
                     */
                    void init() {
                        TRACE_FUNCTION;

                        driver.onReceive.template bind<type, &type::rx_interrupt>(this);
                        driver.init();
                    }

                    /*! \brief  Binds a subject to a specific network name.
                     *
                     *  \param[in]  s   subject of the channel
                     *  \param[out] snn 'Short Network Name'
                     */
                    void bind(const Subject &s, SNN &snn) {
                        TRACE_FUNCTION;
                        snn = s;
                    }

                    /*! \brief  Publishes messages using the %driver.
                     *  \param[in] p  packet to be published
                     *  \todo insert value of p.fragment into message
                     */
                    void deliver(const Packet_t& p) {
                        TRACE_FUNCTION;

                        mob_t smob;
                        SNN *subject = reinterpret_cast<SNN*>( smob.payload );
                        *subject     = p.snn;

                        smob.size = 0;
                        while ( smob.size != p.data_length ) {
                            smob.payload[sizeof( SNN ) + smob.size] = p.data[smob.size];
                            ++smob.size;
                        }
                        smob.size += sizeof( SNN );
                        driver.send( smob );
                    }

                    /*! \brief  Transfers the data of the last received message
                     *          into a packet.
                     *
                     *  \param[out] p   packet for the message data
                     *  \todo get value of p.fragment from message
                     */
                    void take(Packet_t& p) {
                        TRACE_FUNCTION;

                        p.snn         = *reinterpret_cast<SNN*>( rmob.payload );
                        p.data        = rmob.payload + sizeof( SNN );
                        p.data_length = rmob.size - sizeof( SNN );
                        p.fragment    = 0;
                    }

                    /*! \brief  Triggered if a new packet was received by the
                     *          %driver.
                     */
                    void rx_interrupt() {
                        TRACE_FUNCTION;

                        driver.recv(rmob);
                        if (rmob.size != 0)
                            famouso::mw::el::IncommingEventFromNL(this);
                    }

                    /*! \brief  Called if the driver is able to send
                     *          new/further messages.
                     *  \note   This functionality is not implemented yet.
                     *  \todo   Provide functionality to use tx_interrupt
                     */
                    void tx_interrupt() {
                        TRACE_FUNCTION;
                    }

                    /*! \brief  Gets the 'Short Network Name' of the last message.
                     *  \return Returns the 'Short Network Name' of the message
                     *          last received.
                     */
                    SNN lastPacketSNN() {
                        return *reinterpret_cast<SNN*>( rmob.payload );
                    }
            };
        } // namespace nl
    } // namespace mw
} //namespace famouso

#endif /*__SERIAL_NL_h__*/
