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

#include "mw/nl/UDPBroadCastNL.h"
#include "mw/el/EventLayerCallBack.h"

#include "debug.h"
#include <boost/bind.hpp>
#include <boost/asio/ip/multicast.hpp>
#include <boost/asio/placeholders.hpp>
#include "util/CommandLineParameterGenerator.h"
#include "util/ios.h"

namespace famouso {
    namespace mw {
        namespace nl {

            CLP1( UDPOptions,
                  "UDP-BroadCastNL",
                  "broadcast-port,b",
                  "broadcast port (e.g. 8888 (default) ",
                  int, port, 8888
            );

            UDPBroadCastNL::UDPBroadCastNL(): m_socket(famouso::util::ios::instance()) {
                famouso::util::impl::start_ios();
            }

            UDPBroadCastNL::~UDPBroadCastNL() {
                m_socket.close();
            }

            void UDPBroadCastNL::init() {
                CLP::config::UDPOptions::Parameter param;
                CLP::config::UDPOptions::instance().getParameter(param);

                // define a broadcast endpoint with the defined port
                m_endpoint_broadcast=boost::asio::ip::udp::endpoint(
                                        boost::asio::ip::address_v4::broadcast(),
                                        param.port);

                m_socket.open(boost::asio::ip::udp::v4());
                m_socket.set_option(boost::asio::socket_base::broadcast(true));

                // listen on all interface on the defined port for
                // incomming packets
                m_socket.bind(
                    boost::asio::ip::udp::endpoint(
                        boost::asio::ip::udp::v4(),
                        param.port)
                    );

                // bind asynchrone function that is called upon an incomming packet
                m_socket.async_receive_from(
                    boost::asio::buffer(m_buffer, info::mtu), m_endpoint_from,
                    boost::bind(&UDPBroadCastNL::interrupt, this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred
                               )
                );
            }

            void UDPBroadCastNL::bind(const famouso::mw::Subject &s, SNN &snn) {
                snn = s;
            }

            void UDPBroadCastNL::interrupt(const boost::system::error_code& error, size_t bytes_recvd) {
                if (!error) {
                    m_incoming_packet.snn = famouso::mw::Subject(m_buffer) ;
                    m_incoming_packet.fragment = *(bool *)(m_buffer + sizeof(famouso::mw::Subject));
                    m_incoming_packet.data = m_buffer + sizeof(famouso::mw::Subject) + sizeof(bool);
                    m_incoming_packet.data_length = bytes_recvd - sizeof(famouso::mw::Subject) - sizeof(bool);

                    famouso::mw::el::IncommingEventFromNL(this);

                    m_socket.async_receive_from(
                        boost::asio::buffer(m_buffer, info::mtu), m_endpoint_from,
                        boost::bind(&UDPBroadCastNL::interrupt, this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred
                                   ));
                } else {
                    ::logging::log::emit< ::logging::Error>()
                        << "while receiving : " << error.message().c_str()
                        << ::logging::log::endl;
                    throw "Error in UDPBroadCastNL::interrupt";
                }

            }

            void UDPBroadCastNL::take(Packet_t& p) {
                p = m_incoming_packet;
            }

            void UDPBroadCastNL::deliver(const Packet_t& p) {
                bool fragment = p.fragment;
                boost::array<boost::asio::const_buffer, 3> buffers = {{
                    boost::asio::buffer(&p.snn, sizeof(famouso::mw::Subject)),
                    boost::asio::buffer(&fragment, sizeof(bool)),
                    boost::asio::buffer(p.data, p.data_length)
                }};

                m_socket.send_to(buffers,m_endpoint_broadcast);
            }

            UDPBroadCastNL::SNN UDPBroadCastNL::lastPacketSNN() {
                return m_incoming_packet.snn;
            }

        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */
