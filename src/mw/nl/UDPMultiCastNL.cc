/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#include "mw/nl/UDPMultiCastNL.h"
#include "mw/el/EventLayerCallBack.h"

namespace famouso {
    namespace mw {
        namespace nl {

            void UDPMultiCastNL::init() {
                m_socket.open(m_endpoint_listen.protocol());
                m_socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
                // m_socket.set_option( boost::boost::asio::ip::multicast::hops( 3 ) );
                // m_socket.set_option( boost::boost::asio::ip::multicast::enable_loopback(false) );
                m_socket.set_option(boost::asio::ip::multicast::enable_loopback(false));

                m_socket.bind(m_endpoint_listen);

                m_socket.async_receive_from(
                    boost::asio::buffer(m_buffer, info::payload), m_endpoint_from,
                    boost::bind(&UDPMultiCastNL::interrupt, this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred
                               )
                );
            }

            void UDPMultiCastNL::bind(const famouso::mw::Subject &s, SNN &snn) {
                //  private Multicast Adressen: 239.0.0.0 - 239.255.255.255
                // 8 Byte -> 3 Byte
                std::stringstream addr;

                snn.s = s;
                uint8_t temp_ = s.tab()[5];
                if (temp_ == 0) temp_++;
                if (temp_ == 255) temp_--;

                addr << "239."
                << static_cast<short>(s.tab()[0]) << "."
                << static_cast<short>(s.tab()[7]) << "."
                << static_cast<short>(temp_);

                snn.snn = boost::asio::ip::address::from_string(addr.str());
                try {
                    // try to bind the socket on an UDP-MultiCast address
                    m_socket.set_option(boost::asio::ip::multicast::join_group(snn.snn));
                } catch (...) {
                    // this address is already bound which is not an error
                    // that only means that another event channel with the same
                    // subject was bound beforehand
                }
            }

            void UDPMultiCastNL::interrupt(const boost::system::error_code& error, size_t bytes_recvd) {
                if (!error) {
                    m_incoming_packet.snn.snn = boost::asio::ip::address(m_endpoint_from.address());
                    m_incoming_packet.snn.s = famouso::mw::Subject(m_buffer) ;
                    m_incoming_packet.data = m_buffer + sizeof(famouso::mw::Subject);
                    m_incoming_packet.data_length = bytes_recvd - sizeof(famouso::mw::Subject);

                    famouso::mw::el::IncommingEventFromNL(this);
                } else {
                    std::cerr << "while receiving : " << error.message() << std::endl;
                }


                m_socket.async_receive_from(
                    boost::asio::buffer(m_buffer, info::payload), m_endpoint_from,
                    boost::bind(&UDPMultiCastNL::interrupt, this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred
                               )
                );

            }

            void UDPMultiCastNL::fetch(Packet_t& p) {
                p = m_incoming_packet;
            }

            void UDPMultiCastNL::deliver(const Packet_t& p) {
                std::vector<boost::asio::const_buffer> buffers;
                buffers.push_back(boost::asio::buffer(&p.snn.s, sizeof(famouso::mw::Subject)));
                buffers.push_back(boost::asio::buffer(p.data, p.data_length));

                m_socket.async_send_to(
                    buffers,
                    boost::asio::ip::udp::endpoint(p.snn.snn, port),
                    boost::bind(
                        &UDPMultiCastNL::handle_send_to,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred
                    )
                );
            }

            void UDPMultiCastNL::handle_send_to(const boost::system::error_code& error, size_t bytes_recvd) {
                if (!error) {

                } else {
                    std::cerr << "while sending : " << error.message() << std::endl;
                }
            }

            UDPMultiCastNL::SNN UDPMultiCastNL::lastPacketSNN() {
                return m_incoming_packet.snn;
            }

        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */
