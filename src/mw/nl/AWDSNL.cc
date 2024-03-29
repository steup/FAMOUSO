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

#include <vector>

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/placeholders.hpp>

#ifdef RANDOM_ATTRIBUTES
#include <cstdlib>
#endif

#include "mw/nl/awds/logging.h"

#include "util/ios.h"
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
                            "  FLOW_PRIO:\tThe awds flow priority (-10 to 10)\n"
                            "(default 127.0.0.1:8555:60:70:0)",
                            std::string, ip, "127.0.0.1",
                            int, port, 8555,
                            int, interval, 60,
                            int, max_age, 70,
                            int8_t, flow_prio, 0)
            ;

            // broacast address to send a paket to all nodes
            static uint8_t BROADCAST_ADDR[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

            AWDSNL::AWDSNL() :
                m_socket(famouso::util::ios::instance()), timer_(famouso::util::ios::instance()), _repo(NodeRepository::getInstance()),
                                next_packet_is_full_packet(false), flowMgmtAvail(false) {
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
                _maxAge = param.max_age;
                _flowPrio = param.flow_prio;

#ifdef RANDOM_ATTRIBUTES
                // init random generator for attributes generation
                srand(time(NULL));
#endif

                // AWDS endpoint to connect to
                boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(param.ip), param.port);
                boost::system::error_code ec;
                m_socket.connect(endpoint, ec); // connect to AWDS
                if (ec) {
                    // An error occurred.
                    throw "could not connect to AWDS-Network";
                }

                // start getting data from AWDS
                boost::asio::async_read(m_socket, boost::asio::buffer(&awds_packet, sizeof(AWDS_Packet::Header)),
                                        boost::bind(&AWDSNL::interrupt, this, boost::asio::placeholders::error,
                                                    boost::asio::placeholders::bytes_transferred));

            }

            void AWDSNL::bind(const famouso::mw::Subject &s, SNN &snn) {
                snn = s;
            }

            void AWDSNL::deliver(const Packet_t& p) {
                // try to detect subscription channel because in AWDS its get special treatment
                // this test works only due to the fact that the SNN is equal to the famouso::mw::Subject
                if (p.snn == famouso::mw::Subject("SUBSCRIBE")) {
                    SNN s = SNN(p.data);
                    ComparableAttributeSet<>::type attribs;

#ifdef RANDOM_ATTRIBUTES
                    AWDSAttributeSet as = createRandAttributes();
                    attribs = ComparableAttributeSet<>::create(as);
#elif defined TEST_ATTRIBUTES
                    AWDSAttributeSet as = createTestAttributes();
                    attribs = ComparableAttributeSet<>::create(as);
#else
                    // TODO: load Attributes from Packet_t p
                    attribs = ComparableAttributeSet<>::create();
#endif

                    subscriptions[s] = attribs;
                    timer_.cancel();
                    announce_subscriptions(boost::system::error_code());
                } else {
                    log::emit<AWDS>() << "Publish for Subject: " << p.snn << log::endl;

                    {
                        // TODO: Temporary workaround until publisher announcing subjects
                        Attributes::type a = Attributes::create();
#ifdef RANDOM_ATTRIBUTES
                        AWDSAttributeSet as = createRandAttributes();
                        a = Attributes::create(as);
#endif
                        _repo.reg(p.snn, a);
                    }

                    // get list of subscriber for the subject
                    std::pair<NodeRepository::NodeIterator, NodeRepository::NodeIterator> nit = _repo.find(p.snn);

                    // if we have no subscriber, we won't send anything
                    if (nit.first == nit.second) {
                        log::emit<AWDS>() << "No subsciber found." << log::endl;
                        return;
                    }

                    // determine type of packet
                    uint8_t type = p.fragment ? AWDS_Packet::constants::packet_type::publish_fragment
                                              : AWDS_Packet::constants::packet_type::publish;

                    std::vector<boost::asio::const_buffer> buffers;
                    std::list<Node::type> badFlowNodes, lateNodes;
                    AWDS_Packet::Header awds_header;
                    int subs = 0;

                    // set the package params
                    awds_header.type = type;
                    awds_header.size = htons(p.data_length + sizeof(SNN));
                    buffers.push_back(boost::asio::buffer(&awds_header, sizeof(AWDS_Packet::Header)));
                    buffers.push_back(boost::asio::buffer(&p.snn, sizeof(SNN)));
                    buffers.push_back(boost::asio::buffer(p.data, p.data_length));

                    // for each node set source mac and send package
                    for (NodeRepository::NodeIterator it = nit.first; it != nit.second; it++) {
                        Node::type node = *it;

                        if (_repo.elapsed(node, p.snn) > _maxAge) {
                            // Node subscription is missing, we have to unregister the flow id
                            lateNodes.push_back(node);
                        } else if (!flowMgmtAvail || _repo.flowid(node, p.snn) > 0) {
                            // flow id is good or no flow manager available


                            awds_header.addr = node->mac();
                            m_socket.send(buffers);
                            subs++;
                        } else {
                            if (_repo.flowid(node, p.snn) == 0) // no flow id requested yet
                                badFlowNodes.push_back(node);
                        }
                    }

                    // send as unicast
                    log::emit<AWDS>() << "Publishing to " << subs << " subscriber." << log::endl;

                    if (badFlowNodes.size() > 0) {
                        // we have nodes without flow ids
                        buffers.clear();

                        log::emit<AWDS>() << "Requesting " << badFlowNodes.size() << " flow ids." << log::endl;

                        // setup header for flow management
                        awds_header.type = AWDS_Packet::constants::packet_type::flowmgmt;
                        awds_header.size = htons(FlowMgmtRequestAttributeSet::overallSize);
                        buffers.push_back(boost::asio::buffer(&(awds_header), sizeof(AWDS_Packet::Header)));

                        // setup for flow request
                        FlowMgmtRequestAttributeSet aset;
                        aset.find_rt<FlowMgmtAction> ()->setValue(FlowMgmtActionIDs::reg);
                        aset.find_rt<SubjectAttribute> ()->subject(p.snn);
                        aset.find_rt<Priority> ()->setValue(_flowPrio);

                        buffers.push_back(boost::asio::buffer(&(aset), FlowMgmtRequestAttributeSet::overallSize));

                        for (std::list<Node::type>::iterator it = badFlowNodes.begin(); it != badFlowNodes.end(); it++) {
                            Node::type node = *it;

                            // flow id is now requested
                            _repo.update(node, p.snn, -1);

                            // set node to get flow id for
                            awds_header.addr = node->mac();

                            // find attributes for the request
                            // Todo: maybe here we should set some minimal default value, if no attributes are defined
                            _repo.find(node, p.snn, aset);

                            // send packet
                            m_socket.send(buffers);
                        }
                    }

                    if (lateNodes.size() > 0) {
                        // we have nodes with missing subscriptions

                        FlowId fid;
                        FlowMgmtRequestAttributeSet aset;

                        log::emit<AWDS>() << "Unregister " << lateNodes.size() << " nodes." << log::endl;

                        if (flowMgmtAvail) { // we only have to do this when a flow manager is available
                            buffers.clear();

                            // setup header for flow management
                            awds_header.type = AWDS_Packet::constants::packet_type::flowmgmt;
                            awds_header.size = htons(FlowMgmtRequestAttributeSet::overallSize);
                            buffers.push_back(boost::asio::buffer(&(awds_header), sizeof(AWDS_Packet::Header)));

                            // setup for flow free
                            aset.find_rt<FlowMgmtAction> ()->setValue(FlowMgmtActionIDs::free);
                            aset.find_rt<SubjectAttribute> ()->subject(p.snn);
                            buffers.push_back(boost::asio::buffer(&(aset), FlowMgmtRequestAttributeSet::overallSize));
                        }

                        for (std::list<Node::type>::iterator it = lateNodes.begin(); it != lateNodes.end(); it++) {
                            Node::type node = *it;

                            if (flowMgmtAvail) {// we only have to do this when a flow manager is available
                                fid = _repo.flowid(node, p.snn); // get actual flowid
                                if (fid >= 0) {// we have a flow id

                                    // set node to free flow id for
                                    awds_header.addr = node->mac();
                                    // set flow id to free
                                    aset.find_rt<FlowMgmtID> ()->setValue(fid);

                                    // send packet
                                    m_socket.send(buffers);
                                }
                            }

                            // unreg node from subject
                            _repo.unreg(node, p.snn);
                        }
                    }
                }
            }

            void AWDSNL::take(Packet_t& p) {
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
                            // get the node
                            Node::type src = _repo.find(awds_packet.header.addr);

                            log::emit<AWDS>() << "Subscriber: " << src << log::endl;

                            uint16_t pos = 0;

                            // count the number of subjects in the package
                            uint16_t subs_count = *reinterpret_cast<const uint16_t*> (&awds_packet.data[pos]);
                            subs_count = ntohs(subs_count);
                            log::emit<AWDS>() << "Subjects: " << subs_count << log::endl;
                            pos += 2;

                            // for all subjects in package
                            for (uint16_t sub = 0; sub < subs_count; sub++) {

                                // get the subject
                                SNN s = SNN(&awds_packet.data[pos]);
                                pos += sizeof(SNN);
                                Attributes::type attribs = Attributes::create(&awds_packet.data[pos]);
                                pos += attribs->size();

                                log::emit<AWDS>() << "  Subject: " << s << " (" << attribs << ")" << log::endl;

                                // register node to this subject
                                _repo.reg(src, s, attribs);
                            }

                            break;
                        }
                        case AWDS_Packet::constants::packet_type::attributes: {
                            log::emit<AWDS>() << "=============================" << log::dec << log::endl;

                            // get the node
                            Node::type src = _repo.find(awds_packet.header.addr);

                            Attributes::type att = Attributes::create(awds_packet);

                            log::emit<AWDS>() << "Updating attributes of node " << src << log::endl;
                            log::emit<AWDS>() << att << log::endl;

                            // Update attributes of node
                            src->attr(att);
                            break;
                        }

                        case AWDS_Packet::constants::packet_type::flowmgmt: {
                            log::emit<AWDS>() << "=============================" << log::dec << log::endl;

                            FlowMgmtResponseAttributeSet *resp = reinterpret_cast<FlowMgmtResponseAttributeSet *> (awds_packet.data);
                            FlowMgmtAction *fa = resp->find_rt<FlowMgmtAction> ();
                            FlowMgmtID *id = resp->find_rt<FlowMgmtID> ();

                            if (!id || !fa)
                                throw "Missing Action or FlowId Attribute from AWDS FlowManagement!";

                            if (fa->getValue() == FlowMgmtActionIDs::avail) {
                                if (id->getValue() > 0) {
                                    log::emit<AWDS>() << "Flow manager available." << log::endl;
                                    flowMgmtAvail = true;
                                } else {
                                    log::emit<AWDS>() << "Flow manager not available." << log::endl;
                                    flowMgmtAvail = false;
                                }
                                break;
                            }

                            // get the Node
                            Node::type src = _repo.find(awds_packet.header.addr);

                            log::emit<AWDS>() << "Updating flow id of node " << src << log::endl;

                            SubjectAttribute *sub = resp->find_rt<SubjectAttribute> ();
                            if (!sub)
                                throw "Missing Subject Attribute from AWDS FlowManagement!";

                            log::emit<AWDS>() << "New ID: " << (FlowId) id->getValue() << (fa->getValue() == FlowMgmtActionIDs::use ? " OK!"
                                                                                                                          : " Bad!")
                                            << log::endl;

                            // set new flow Id
                            _repo.update(src, sub->subject(), id->getValue() > 0 ? id->getValue() : 0);
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
                        std::vector<boost::asio::const_buffer> buffers;
                        AWDS_Packet::Header awds_header;
                        uint16_t size = sizeof(uint16_t);
                        uint16_t subs = 0;

                        awds_header.addr = MAC::parse(BROADCAST_ADDR);
                        awds_header.type = AWDS_Packet::constants::packet_type::subscribe;
                        buffers.push_back(boost::asio::buffer(&awds_header, sizeof(AWDS_Packet::Header)));
                        buffers.push_back(boost::asio::buffer(&subs, sizeof(uint16_t)));

                        for (std::map<SNN, ComparableAttributeSet<>::type>::const_iterator i = subscriptions.begin(); i
                                        != subscriptions.end(); ++i) {
                            ComparableAttributeSet<>::type attr = i->second;

                            int size2 = sizeof(SNN) + attr->size();
                            if (size + size2 > AWDS_Packet::constants::packet_size::payload) {
                                // packet is full, so we have to send it and make a new one
                                subs = htons(subs);
                                awds_header.size = htons(size);
                                m_socket.send(buffers);

                                // prepare a new packet
                                size = sizeof(uint16_t);
                                subs = 0;
                                buffers.clear();
                                buffers.push_back(boost::asio::buffer(&awds_header, sizeof(AWDS_Packet::Header)));
                                buffers.push_back(boost::asio::buffer(&subs, sizeof(uint16_t)));
                            }

                            buffers.push_back(boost::asio::buffer(&(i->first), sizeof(SNN)));
                            buffers.push_back(*attr);

                            size += size2;
                            subs++;
                        }

                        // send last subscription packet
                        subs = htons(subs);
                        awds_header.size = htons(size);
                        m_socket.send(buffers);
                    }

                    timer_.expires_from_now(boost::posix_time::seconds(interval));
                    timer_.async_wait(boost::bind(&AWDSNL::announce_subscriptions, this, boost::asio::placeholders::error));
                }
            }
        }
    }
}

