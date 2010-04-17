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

#ifndef __UDPMultiCastNL_hpp__
#define __UDPMultiCastNL_hpp__

#include <stdio.h>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <string>

#include "mw/nl/BaseNL.h"
#include "mw/nl/Packet.h"
#include "util/ios.h"
#include "mw/common/Subject.h"

namespace famouso {
    namespace mw {
        namespace nl {

            class UDPMultiCastNL : public BaseNL, boost::noncopyable {
                public:

                    struct info {
                        enum {
                            mtu = 8192,
                            payload = mtu - sizeof(famouso::mw::Subject),
                        };
                    };

                    // type of an address
                    struct SNN {
                        boost::asio::ip::address snn;
                        famouso::mw::Subject s;
                        bool operator == (const SNN &s1) const {
                            return s == s1.s;
                        }
                    };

                    // type of a packet
                    typedef Packet<SNN> Packet_t;

                    /**
                     * @brief default constructor
                     *
                     * Sets local variables and calls init.
                     */
                    UDPMultiCastNL();

                    /**
                     * @brief destructor
                     *
                     *  Closes the socket.
                     */
                    ~UDPMultiCastNL();

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
                     * @brief Sends the given packet.
                     *
                     * @param p packet
                     */
                    void deliver_fragment(const Packet_t& p) {
                        deliver(p);
                    };

                    /**
                     * @brief processes incoming packets
                     *
                     * @param p fetched packet is saved here
                     */
                    void fetch(Packet_t& p);

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
                    boost::asio::ip::udp::endpoint m_endpoint_listen;
                    boost::asio::ip::udp::endpoint m_endpoint_from;
                    uint8_t m_buffer[info::mtu];   // Subject and Payload have to be sent
                    Packet_t m_incoming_packet;
            };

        }
    }
} // namespaces


#endif /* __UDPMultiCastNL_hpp__ */
