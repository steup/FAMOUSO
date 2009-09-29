/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>,
 *                          Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __EventLayerClientStub_h__
#define __EventLayerClientStub_h__

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "mw/api/SubscriberEventChannel.h"
#include "mw/api/PublisherEventChannel.h"

#include "mw/el/LocalConnectionParameters.h"
#include "util/endianness.h"
#include "case/Delegate.h"
#include "util/ios.h"
#include "debug.h"

#include <map>

namespace famouso {
    namespace mw {
        namespace el {

            template< class T >
            class NotifyWorkerThread {

                    T &sec;
                public:
                    void action() {
                        uint8_t recvBuffer[BUFSIZE];
                        int recvMsgSize;
                        try {
                            while ((recvMsgSize = sec.snn()->receive(boost::asio::buffer(recvBuffer, 13))) > 0) {
                                // ermitteln der Laenge des Events
                                unsigned int len = ntohl(*(uint32_t *) & (recvBuffer[9]));
                                // und den Rest aus dem Socket holen
                                if ((recvMsgSize = sec.snn()->receive(boost::asio::buffer(recvBuffer, len))) > 0) {
                                    // Event aufbauen und veroeffentlichen
                                    Event e(sec.subject());
                                    e.length = len;
                                    e.data = (uint8_t *) recvBuffer;
                                    sec.callback(e);
                                }
                            }
                        } catch (...) {
                            // unsubscribe will disallow receive -> exception to stop thread
                        }
                    }
                public:
                    NotifyWorkerThread(T &ec) : sec(ec) {
                    }
            };

            class EventLayerClientStub {

                    typedef famouso::mw::api::SubscriberEventChannel<EventLayerClientStub>  SEC;
                    boost::array<char, 13> event_head;
                    boost::array<char, 65535> event_data;
                    typedef std::pair<boost::thread *, NotifyWorkerThread<SEC> *> NotifyThreadData;
                    typedef std::map<SEC *, NotifyThreadData> NotifyThreadMap;
                    NotifyThreadMap notify_threads;
                    boost::mutex notify_threads_mutex;

                    void do_connection_socket(famouso::mw::api::EventChannel<EventLayerClientStub> &ec) {
                        ec.snn() = new boost::asio::ip::tcp::socket(famouso::util::ios::instance());
                        // Establish connection with the ech
                        boost::asio::ip::tcp::endpoint endpoint(
                            boost::asio::ip::address::from_string(servAddress), ServPort);

                        try {
                            ec.snn()->connect(endpoint);
                        } catch (...) {
                            ::logging::log::emit< ::logging::Error>()
                                << "An error occurred while connecting to the ech"
                                << ::logging::log::endl;
                            abort();
                        }
                    }
                public:
                    typedef boost::asio::ip::tcp::socket *SNN;

                    void init() {
                        famouso::util::impl::start_ios();
                    }

                    EventLayerClientStub() {
                        init();
                    }

                    // announce legt hier nur einen Socket an und meldet sich
                    // bei localen EventChannelHandler an
                    void announce(famouso::mw::api::PublisherEventChannel<EventLayerClientStub> &ec) {
                        TRACE_FUNCTION;
                        do_connection_socket(ec);
                        uint8_t transferBuffer[9] = {FAMOUSO::ANNOUNCE};
                        for (uint8_t i = 0;i < 8;++i)
                            transferBuffer[i+1] = ec.subject().tab()[i];
                        // Send the announcement to the ech
                        boost::asio::write(*ec.snn(), boost::asio::buffer(transferBuffer, sizeof(transferBuffer)));
                    }

                    // Publish uebermittelt die Daten
                    // an den localen ECH
                    void publish(famouso::mw::api::PublisherEventChannel<EventLayerClientStub> &ec, const Event &e) {
                        TRACE_FUNCTION;
                        ::logging::log::emit< ::logging::Info>() << "Publish channel "
                                 << ::logging::log::hex << ec.select() << ::logging::log::endl;
                        // Hier koennte der Test erfolgen, ob die Subjects vom Event
                        // und vom EventChannel gleich sind, weil nur dann
                        // das Event in diesen Kanal gehoert
                        //
                        // Ist mit einem assert zu machen und auch ob das Subject des
                        // Kanals ueberhaupt gebunden ist, aber dies ist einfach, weil sonst
                        // keine Verbindung zum ech besteht und das send fehlschlaegt
                        uint8_t transferBuffer[13] = {FAMOUSO::PUBLISH};
                        for (uint8_t i = 0;i < 8;++i)
                            transferBuffer[i+1] = ec.subject().tab()[i];
                        uint32_t *len = (uint32_t *) & transferBuffer[9];
                        *len = htonl(e.length);
                        // Send the announcement to the ech
                        boost::asio::write(*ec.snn(), boost::asio::buffer(transferBuffer, sizeof(transferBuffer)));
                        boost::asio::write(*ec.snn(), boost::asio::buffer(e.data, e.length));
                    }

                    // Verbindung  zum  ECH oeffnen und Subject subscribieren
                    void subscribe(famouso::mw::api::SubscriberEventChannel<EventLayerClientStub> &ec) {
                        TRACE_FUNCTION;
                        ::logging::log::emit< ::logging::Info>() << "Subscribe channel "
                                 << ::logging::log::hex << ec.select() << ::logging::log::endl;
                        do_connection_socket(ec);
                        // create subscribe message
                        uint8_t transferBuffer[9] = {FAMOUSO::SUBSCRIBE};
                        for (uint8_t i = 0;i < 8;++i)
                            transferBuffer[i+1] = ec.subject().tab()[i];
                        // Send the announcement to the ech
                        boost::asio::write(*ec.snn(), boost::asio::buffer(transferBuffer, sizeof(transferBuffer)));
                        // create a thread that gets the ec and if a messages arrives at the
                        // socket connection the ec is called back
                        NotifyWorkerThread<SEC> * nwt = new NotifyWorkerThread<SEC>(ec);
                        NotifyThreadData t (new boost::thread(boost::bind(&NotifyWorkerThread<SEC>::action, nwt)), nwt);
                        notify_threads_mutex.lock();
                        notify_threads[&ec] = t;
                        notify_threads_mutex.unlock();
                        ::logging::log::emit< ::logging::Info>()
                            << "Generate Thread and Connect to local ECH"
                            << ::logging::log::endl;
                    }

                    // Verbindung schliessen, sollte reichen
                    void unsubscribe(famouso::mw::api::SubscriberEventChannel<EventLayerClientStub> &ec) {
                        TRACE_FUNCTION;
                        ::logging::log::emit< ::logging::Info>() << "close connection" << ::logging::log::endl;

                        // Only return when notify thread terminated to prevent the thread to
                        // access already deleted data structures resulting in undefined behaviour
                        notify_threads_mutex.lock();
                        NotifyThreadMap::iterator it = notify_threads.find(&ec);
                        if (it != notify_threads.end()) {
                            NotifyThreadData t = it->second;
                            ec.snn()->shutdown(boost::asio::ip::tcp::socket::shutdown_receive);
                            t.first->join();
                            ec.snn()->close();
                            delete t.first;
                            delete t.second;
                            notify_threads.erase(&ec);
                        }
                        notify_threads_mutex.unlock();
                    }

                    // Verbindung schliessen sollte reichen
                    void unannounce(famouso::mw::api::PublisherEventChannel<EventLayerClientStub> &ec) {
                        TRACE_FUNCTION;
                        ::logging::log::emit< ::logging::Info>() << "close connection" << ::logging::log::endl;
                        ec.snn()->close();
                    }
            };
        } // namespace el
    } // namespace mw
} //namespace famouso

#endif
