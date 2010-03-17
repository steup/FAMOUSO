/*******************************************************************************
 *
 * Copyright (c) 2008-2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *               2009-2010 Michael Kriese <kriese@cs.uni-magdeburg.de>
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

#include "mw/nl/AWDSNL.h"
#include "mw/el/EventLayerCallBack.h"

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "mw/nl/awds/logging.h"

#include "util/CommandLineParameterGenerator.h"

namespace famouso {
    namespace mw {
        namespace nl {
            // define command line parameter and default values
            CLP5(AWDSOptions,
                            "AWDS Network Layer",
                            "awds,a",
                            "connection and config parameter for the awds-network in the form of IP:PORT:INTERVAL:MAX_AGE:MAX_UNI\n"
                            "Values:\n"
                            "  IP:       \tThe ip-addresse of the ps-awds-deamon\n"
                            "  PORT:     \tThe port on which the deamon listen\n"
                            "  INTERVAL: \tThe renew-interval of subsciptions\n"
                            "  MAX_AGE:  \tThe max age of subsciptions in seconds\n"
                            "  MAX_UNI:  \tThe max clients count to send unicast packages\n"
                            "(default 127.0.0.1:8555:60:70:5)",
                            std::string, ip, "127.0.0.1",
                            int, port, 8555,
                            int, interval, 60,
                            int, max_age, 70,
                            uint, max_uni, 5)
            ;

            AWDSNL::AWDSNL() :
                m_socket(famouso::util::ios::instance()), timer_(famouso::util::ios::instance()), _repo(ClientRepository::getInstance()),
                                next_packet_is_full_packet(false) {
            }

            AWDSNL::~AWDSNL() throw () {
                m_socket.close();
            }

            void AWDSNL::init() {
                famouso::util::impl::start_ios();

                // get command line parameter
                CLP::config::AWDSOptions::Parameter param;
                CLP::config::AWDSOptions::instance().getParameter(param);

                // set local variables
                interval = param.interval;
                max_unicast = param.max_uni;
                _repo.maxAge(param.max_age);

                boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(param.ip), param.port);
                boost::system::error_code ec;
                m_socket.connect(endpoint, ec);
                if (ec) {
                    // An error occurred.
                    throw "could not connect to AWDS-Network";
                }

                boost::asio::async_read(m_socket, boost::asio::buffer(&awds_packet, sizeof(AWDS_Packet::Header)),
                                        boost::bind(&AWDSNL::interrupt, this, boost::asio::placeholders::error,
                                                    boost::asio::placeholders::bytes_transferred));

            }

            void AWDSNL::bind(const famouso::mw::Subject &s, SNN &snn) {
                snn = s;
            }

            void AWDSNL::deliver(const Packet_t& p, uint8_t type) {
                // try to detect subscription channel because in AWDS its get special treatment
                // this test works only due to the fact that the SNN is equal to the famouso::mw::Subject
                if (p.snn == famouso::mw::Subject("SUBSCRIBE")) {
                    subscriptions.push_front(famouso::mw::Subject(p.data));
                    subscriptions.sort();
                    subscriptions.unique();

                    timer_.cancel();
                    announce_subscriptions(boost::system::error_code());
                } else {

                    // get list of subscriber for the subject
                    ClientRepository::ClientList::type cl = _repo.find(p.snn);

                    // if we have no subscriber, we won't send anything
                    if (cl->size() == 0) {
                        log::emit<AWDS>() << "No subsciber for Subject: " << p.snn << log::endl;
                        return;
                    }

                    std::vector<boost::asio::const_buffer> buffers;
                    AWDS_Packet::Header awds_header;

                    // only broadcast if we have to much subscribers
                    if (cl->size() > max_unicast) {
                        // send as broadcast
                        log::emit<AWDS>() << "Broadcast (" << cl->size() << ") \t -- Subject: " << p.snn << log::endl;

                        // set the package params
                        awds_header.addr[0] = 0xff;
                        awds_header.addr[1] = 0xff;
                        awds_header.addr[2] = 0xff;
                        awds_header.addr[3] = 0xff;
                        awds_header.addr[4] = 0xff;
                        awds_header.addr[5] = 0xff;
                        awds_header.type = type;
                        awds_header.size = htons(p.data_length + sizeof(famouso::mw::Subject));
                        buffers.push_back(boost::asio::buffer(&awds_header, sizeof(AWDS_Packet::Header)));
                        buffers.push_back(boost::asio::buffer(&p.snn, sizeof(famouso::mw::Subject)));
                        buffers.push_back(boost::asio::buffer(p.data, p.data_length));

                        // send
                        m_socket.send(buffers);
                    } else {
                        // send as unicast
                        log::emit<AWDS>() << "Unicast (" << cl->size() << ") \t -- Subject: " << p.snn << log::endl;

                        // set the package params
                        awds_header.type = type;
                        awds_header.size = htons(p.data_length + sizeof(SNN));
                        buffers.push_back(boost::asio::buffer(&awds_header, sizeof(AWDS_Packet::Header)));
                        buffers.push_back(boost::asio::buffer(&p.snn, sizeof(SNN)));
                        buffers.push_back(boost::asio::buffer(p.data, p.data_length));

                        // for each client set source mac and send package
                        for (ClientRepository::ClientList::iterator it = cl->begin(); it != cl->end(); it++) {
                            (*it)->mac(awds_header.addr);
                            m_socket.send(buffers);
                        }
                    }
                }
            }

            void AWDSNL::deliver_fragment(const Packet_t& p) {
                deliver(p, AWDS_Packet::constants::packet_type::publish_fragment);
            }

            void AWDSNL::fetch(Packet_t& p) {
                p.snn = famouso::mw::Subject(awds_packet.data);
                p.data = &awds_packet.data[8];
                p.data_length = ntohs(awds_packet.header.size) - sizeof(famouso::mw::Subject);
            }

            AWDSNL::SNN AWDSNL::lastPacketSNN() {
                return *reinterpret_cast<SNN*> (awds_packet.data);
            }

            void AWDSNL::interrupt(const boost::system::error_code& error, size_t bytes_recvd) {
                if (!error) {
                    if (!next_packet_is_full_packet) {
                        boost::asio::async_read(m_socket, boost::asio::buffer(&awds_packet.data, ntohs(awds_packet.header.size)),
                                                boost::bind(&AWDSNL::interrupt, this, boost::asio::placeholders::error,
                                                            boost::asio::placeholders::bytes_transferred));
                        next_packet_is_full_packet = true;
                        return;
                    }
                    // we got a full packet and now we will process it
                    switch (awds_packet.header.type) {
                        case AWDS_Packet::constants::packet_type::publish:
                        case AWDS_Packet::constants::packet_type::publish_fragment: {
                            famouso::mw::el::IncommingEventFromNL(this);
                            break;
                        }
                        case AWDS_Packet::constants::packet_type::subscribe: {

                            log::emit<AWDS>() << "=============================" << log::dec << log::endl;
                            // get the client
                            MAC mac = MAC::parse(awds_packet.header.addr);
                            AWDSClient::type src = _repo.find(mac);

                            // reset contact time
                            src->reset();
                            _repo.unreg(src);
                            log::emit<AWDS>() << "Subscriber: " << src << log::endl;

                            // count the number of subjects in the package
                            uint16_t subs_count = *reinterpret_cast<const uint16_t*> (awds_packet.data + 4);
                            subs_count = ntohs(subs_count);
                            log::emit<AWDS>() << "Subjects: " << subs_count << log::endl;

                            // for all subjects in package
                            for (uint16_t sub = 0; sub < subs_count; sub++) {
                                // get the subject
                                SNN s = SNN(awds_packet.data + 6 + (sub * sizeof(SNN)));
                                log::emit<AWDS>() << "  Subject: " << s << log::endl;

                                // register client to this subject
                                _repo.reg(src, s);
                            }

                            log::emit<AWDS>() << "=============================" << log::endl;
                            break;
                        }
                        default:
                            log::emit<AWDS>() << "AWDS_Packet not supported yet" << log::endl;
                            break;
                    }

                    // try receiving the next packet
                    boost::asio::async_read(m_socket, boost::asio::buffer(&awds_packet, sizeof(AWDS_Packet::Header)),
                                            boost::bind(&AWDSNL::interrupt, this, boost::asio::placeholders::error,
                                                        boost::asio::placeholders::bytes_transferred));

                } else {
                    log::emit<Error>() << "AWDS-Network : " << error.message().c_str() << log::endl;
                    throw "AWDS-Network disconnected likely";
                }

                // the next packet is maybe not a full packet
                next_packet_is_full_packet = false;
            }

            void AWDSNL::announce_subscriptions(const boost::system::error_code& error) {
                if (!error) {// != boost::asio::error::operation_aborted)

                    if (subscriptions.size()) {
                        uint16_t count_subj = htons(subscriptions.size());
                        std::vector<boost::asio::const_buffer> buffers;
                        uint32_t reserved = htonl(20);
                        AWDS_Packet::Header awds_header;
                        awds_header.addr[0] = 0xff;
                        awds_header.addr[1] = 0xff;
                        awds_header.addr[2] = 0xff;
                        awds_header.addr[3] = 0xff;
                        awds_header.addr[4] = 0xff;
                        awds_header.addr[5] = 0xff;
                        awds_header.type = AWDS_Packet::constants::packet_type::subscribe;
                        awds_header.size = htons(sizeof(uint32_t) + 2 + ntohs(count_subj) * sizeof(famouso::mw::Subject));
                        buffers.push_back(boost::asio::buffer(&awds_header, sizeof(AWDS_Packet::Header)));
                        buffers.push_back(boost::asio::buffer(&reserved, sizeof(uint32_t)));
                        buffers.push_back(boost::asio::buffer(&count_subj, sizeof(uint16_t)));

                        for (std::list<SNN>::const_iterator i = subscriptions.begin(); i != subscriptions.end(); ++i) {
                            buffers.push_back(boost::asio::buffer(&(*i), sizeof(famouso::mw::Subject)));
                        }

                        m_socket.send(buffers);
                    }

                    timer_.expires_from_now(boost::posix_time::seconds(interval));
                    timer_.async_wait(boost::bind(&AWDSNL::announce_subscriptions, this, boost::asio::placeholders::error));
                }
            }
        }
    }
}

