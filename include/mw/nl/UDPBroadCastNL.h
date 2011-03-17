/*******************************************************************************
 *
 * Copyright (c) 2008-2011 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __UDPBroadCastNL_h__
#define __UDPBroadCastNL_h__

#include "mw/nl/DistinctNL.h"
#include "mw/nl/Packet.h"
#include "mw/afp/Config.h"
#include "mw/common/Subject.h"
#include <boost/asio/ip/udp.hpp>

namespace famouso {
    namespace mw {
        namespace nl {

            class UDPBroadCastNL : public DistinctNL {
                public:
                    struct info {
                        enum {
                            mtu = 8192,
                            payload = mtu - sizeof(famouso::mw::Subject) - sizeof(bool),
                        };
                    };

                    typedef famouso::mw::Subject SNN;

                    typedef Packet<SNN> Packet_t;

                    /// Default AFP %configuration of this network layer (see \ref afp_config_nl)
                    typedef afp::MultiSubjectESeqReorderDuplicateConfig<SNN> AFP_Config;

                    /**
                     * @brief default constructor
                     *
                     * Sets local variables and calls init.
                     */
                    UDPBroadCastNL();

                    /**
                     * @brief destructor
                     *
                     *  Closes the socket.
                     */
                    ~UDPBroadCastNL();

                    /**
                     * Sets the options for the socket and starts receiving.
                     */
                    void init();

                    /**
                     * @brief subscribe a subject
                     *
                     * Start listening on the multicast-address that belongs to the subject.
                     *
                     * @param s subject
                     * @param snn bound address
                     */
                    void bind(const famouso::mw::Subject &s, SNN &snn);

                    /**
                     * @brief Sends the given packet.
                     *
                     * @param p packet
                     */
                    void deliver(const Packet_t& p);

                    /**
                     * @brief processes incoming packets
                     *
                     * @param[out] p   packet that is filled with the received data
                     */
                    void take(Packet_t& p);

                    /**
                     * @brief get last SSN
                     *
                     * Returns the short network name for the last packet.
                     */
                    SNN lastPacketSNN();


                    /**
                     * @brief handle called on receive
                     *
                     * Will be called, whenever a packet was received.
                     *
                     */
                    void interrupt(const boost::system::error_code& error, size_t bytes_recvd);

                private:
                    boost::asio::ip::udp::socket m_socket;
                    boost::asio::ip::udp::endpoint m_endpoint_broadcast;
                    boost::asio::ip::udp::endpoint m_endpoint_from;
                    uint8_t m_buffer[info::mtu];   // Subject and Payload have to be sent
                    Packet_t m_incoming_packet;
            };
        }
    }
}
#endif /* __UDPBroadCastNL_h__ */

