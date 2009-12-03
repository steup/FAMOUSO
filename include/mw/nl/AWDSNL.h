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

#ifndef __AWDSNL_hpp__
#define __AWDSNL_hpp__

#include <stdio.h>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>
#include <vector>
#include <iterator>

#include "debug.h"
#include "mw/nl/BaseNL.h"
#include "mw/nl/Packet.h"
#include "util/ios.h"
#include "util/CommandLineParameterGenerator.h"
#include "mw/common/Subject.h"

#include "mw/el/EventLayerCallBack.h"

namespace famouso {
    namespace mw {
        namespace nl {

            // define command line parameter and default values
            CLP3(AWDSOptions,
                "AWDS Network Layer",
                "awds,a",
                "connection and config parameter for the awds-network in the form of IP:PORT:INTERVAL\n"
                "Values:\n"
                "  IP:       \tThe ip-addresse of the ps-awds-deamon\n"
                "  PORT:     \tThe port on which the deamon listen\n"
                "  INTERVAL: \tThe renew-interval of subsciptions\n"
                "(default 127.0.0.1:8555:60)",
                std::string, ip, "127.0.0.1",
                int, port, 8555,
                int, interval, 60);

            struct __attribute__((packed)) AWDS_Packet {
                struct constants {
                    struct packet_size {
                        enum {
                            payload = 1400
                        };
                    };
                    struct packet_type {
                        enum {
                            publish_fragment = 0x7B,
                            publish = 0x7C,
                            subscribe = 0x7D
                        };
                    };
                };
                struct __attribute__((packed)) Header {
                        uint8_t addr[6];
                        uint8_t type;
                    private:
                        uint8_t __pad;
                    public:
                        uint16_t size;
                };

                Header   header;
                uint8_t  data[constants::packet_size::payload];
            };

            class AWDSNL : public BaseNL, boost::noncopyable {
                public:

                    struct info {
                        enum {
                            payload = AWDS_Packet::constants::packet_size::payload - sizeof(famouso::mw::Subject),
                        };
                    };

                    // type of an address
                    typedef famouso::mw::Subject SNN;

                    // type of a packet
                    typedef Packet<SNN> Packet_t;

                    /**
                     * \brief default constructor
                     */
                    AWDSNL() :  m_socket(famouso::util::ios::instance()),
                                timer_(famouso::util::ios::instance()),
                                next_packet_is_full_packet(false) {}

                    /**
                     * \brief destructor
                     *
                     *  Closes the socket.
                     *
                     */
                    ~AWDSNL() throw() {
                        m_socket.close();
                    }

                    /**
                     * Sets the options for the socket and starts receiving.
                     */
                    void init() {
                        famouso::util::impl::start_ios();
                        // get command line parameter
                        CLP::config::AWDSOptions::instance().getParameter(param);

                        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(param.ip), param.port);
                        boost::system::error_code ec;
                        m_socket.connect(endpoint, ec);
                        if (ec) {
                            // An error occurred.
                            throw "could not connect to AWDS-Network";
                        }

                        boost::asio::async_read( m_socket,
                            boost::asio::buffer(&awds_packet, sizeof(AWDS_Packet)),
                            boost::asio::transfer_at_least(sizeof(AWDS_Packet::Header)),
                            boost::bind(&AWDSNL::interrupt, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred)
                        );

                    }

                    /**
                     * \brief bind a subject
                     *
                     * \param s subject
                     * \param snn bound address
                     */
                    void bind(const famouso::mw::Subject &s, SNN &snn) {
                        snn = s;
                    }

                    /**
                     * \brief deliver
                     *
                     * Sends the given packet.
                     *
                     * \param[in] p packet
                     * \param[in] type describes the packet type and is used for specifying
                     *            a fragmented or normal published packet.
                     *
                     * \todo momentan nur broadcast. siehe spezification fuer andere Verfahren unter doc/psawds/ipc-protocol.txt
                     */
                    void deliver(const Packet_t& p, uint8_t type = AWDS_Packet::constants::packet_type::publish) {
                        // try to detect subscription channel because in AWDS its get special treatment
                        // this test works only due to the fact that the SNN is equal to the famouso::mw::Subject
                        if (p.snn == famouso::mw::Subject("SUBSCRIBE")) {
                            subscriptions.push_front(famouso::mw::Subject(p.data));
                            subscriptions.sort();
                            subscriptions.unique();

                            timer_.cancel();
                            announce_subscriptions(boost::system::error_code());
                        } else {
                            std::vector<boost::asio::const_buffer> buffers;
                            AWDS_Packet::Header awds_header;
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

                            m_socket.send(buffers);
                        }
                    }

                    /**
                     * \brief publish
                     *
                     * Sends the given packet.
                     *
                     * \param p packet
                     */
                    void deliver_fragment(const Packet_t& p) {
                        deliver(p, AWDS_Packet::constants::packet_type::publish_fragment);
                    };

                    /**
                     * \brief processes incoming packets
                     *
                     * \param p fetched packet is saved here
                     */
                    void fetch(Packet_t& p) {
                        p.snn = famouso::mw::Subject(awds_packet.data);
                        p.data =  &awds_packet.data[8];
                        p.data_length = ntohs(awds_packet.header.size) - sizeof(famouso::mw::Subject);
                    }

                    /**
                     * \brief get last SSN
                     *
                     * Returns the short network name for the last packet.
                     */
                    SNN lastPacketSNN() {
                        return *reinterpret_cast<SNN*>(awds_packet.data);
                    }


                    /**
                     * \brief handle called on receive
                     *
                     * Will be called, whenever a packet was received.
                     */
                    void interrupt(const boost::system::error_code& error, size_t bytes_recvd) {
                        if (!error) {
                            if (!next_packet_is_full_packet) {
                               bytes_recvd -= sizeof(AWDS_Packet::Header);
                                // test  whether the packet is full received
                                if ( bytes_recvd != awds_packet.header.size) {
                                    // no and now receive the rest of the packet
                                    boost::asio::async_read( m_socket,
                                        boost::asio::buffer(&awds_packet, sizeof(AWDS_Packet)),
                                        boost::asio::transfer_at_least(awds_packet.header.size-bytes_recvd),
                                        boost::bind(&AWDSNL::interrupt, this,
                                                    boost::asio::placeholders::error,
                                                    boost::asio::placeholders::bytes_transferred)
                                    );
                                    next_packet_is_full_packet=true;
                                    return;
                                }
                            }
                            // we got a full packet and now we will process it
                            switch (awds_packet.header.type) {
                                case AWDS_Packet::constants::packet_type::publish :
                                case AWDS_Packet::constants::packet_type::publish_fragment : {
                                        famouso::mw::el::IncommingEventFromNL(this);
                                        break;
                                    }
                                case AWDS_Packet::constants::packet_type::subscribe : {
                                        /*! \todo implement subscription announcement see protocol specification in doc */
                                        break;
                                    }
                                default: ::logging::log::emit() << "AWDS_Packet not supported yet" << ::logging::log::endl;
                            }

                            // try receiving the next packet
                            boost::asio::async_read( m_socket,
                                boost::asio::buffer(&awds_packet, sizeof(AWDS_Packet)),
                                boost::asio::transfer_at_least(sizeof(AWDS_Packet::Header)),
                                boost::bind(&AWDSNL::interrupt, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred)
                            );

                        } else {
                            ::logging::log::emit< ::logging::Error>() << "AWDS-Network : "
                                << error.message().c_str() << ::logging::log::endl;
                            throw "AWDS-Network disconnected likely";
                        }

                        // the next packet is maybe not a full packet
                        next_packet_is_full_packet=false;

                    }

                private:
                    void announce_subscriptions(const boost::system::error_code& error) {
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

                            timer_.expires_from_now(boost::posix_time::seconds(param.interval));
                            timer_.async_wait(boost::bind(&AWDSNL::announce_subscriptions, this, boost::asio::placeholders::error));
                        }
                    }
                    boost::asio::ip::tcp::socket m_socket;
                    boost::asio::deadline_timer timer_;
                    AWDS_Packet awds_packet;
                    std::list<SNN> subscriptions;
                    CLP::config::AWDSOptions::Parameter param;
                    bool next_packet_is_full_packet;
            };

        }
    }
} // namespaces

#endif /* __AWDSNL_hpp__ */
