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

#ifndef __EventLayerMiddlewareStub_h__
#define __EventLayerMiddlewareStub_h__
//
//#include <ctime>
//#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
//#include <boost/thread/xtime.hpp>
#include <boost/pool/pool.hpp>

#include <iostream>
//#include <list>

#include "mw/common/Event.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "mw/el/LocalConnectionParameters.h"
#include "util/endianness.h"
#include "util/ios.h"

//#include "mw/common/UID.h"


namespace famouso {
    namespace mw {
        namespace el {

            template <typename EL>
            class EventLayerMiddlewareStub {

                    // Pool memory allocator returning shared_ptr deleting themself. Not thread-safe ATM!
                    class EventMemoryPool {

                            // Change this to singleton_pool for thread-safe implementation.
                            typedef boost::pool<boost::default_user_allocator_malloc_free> pool_type;

                            // Use multiple pools of different size to save memory
                            static pool_type & event_preamble() {
                                static pool_type p(13); return p;
                            }
                            struct event_preamble_deleter {
                                void operator()(uint8_t * p) {
                                    event_preamble().free(p);
                                }
                            };

                            static pool_type & event_data_4k() {
                                static pool_type p(4*1024); return p;
                            }
                            struct event_data_4k_deleter {
                                void operator()(uint8_t * p) {
                                    event_data_4k().free(p);
                                }
                            };

                            static pool_type & event_data_16k() {
                                static pool_type p(16*1024); return p;
                            }
                            struct event_data_16k_deleter {
                                void operator()(uint8_t * p) {
                                    event_data_16k().free(p);
                                }
                            };

                            static pool_type & event_data_64k() {
                                static pool_type p(64*1024); return p;
                            }
                            struct event_data_64k_deleter {
                                void operator()(uint8_t * p) {
                                    event_data_64k().free(p);
                                }
                            };

                        public:
                            typedef boost::shared_ptr<uint8_t> pointer;

                            static pointer alloc(size_t size) {
                                if (size <= 13) {
                                    // TODO: use allocate_shared for performance improvments
                                    pointer p((uint8_t *)event_preamble().malloc(), event_preamble_deleter());
                                    return p;
                                } else if (size <= 4*1024) {
                                    pointer p((uint8_t *)event_data_4k().malloc(), event_data_4k_deleter());
                                    return p;
                                } else if (size <= 16*1024) {
                                    pointer p((uint8_t *)event_data_16k().malloc(), event_data_16k_deleter());
                                    return p;
                                } else {
                                    pointer p((uint8_t *)event_data_64k().malloc(), event_data_64k_deleter());
                                    return p;
                                }
                            }
                            // maybe call release_memory() sometimes
                    };


                    template <typename lECH>
                class EventChannelConnection : public boost::enable_shared_from_this< EventChannelConnection<lECH> > {
                            typedef famouso::mw::api::PublisherEventChannel<lECH> PEC;
                            typedef famouso::mw::api::SubscriberEventChannel<lECH> SEC;

                        public:
                            typedef boost::shared_ptr<EventChannelConnection> pointer;

                            static pointer create() {
                                return pointer(new EventChannelConnection());
                            }

                            boost::asio::ip::tcp::socket& socket() {
                                return socket_;
                            }

                            void start() {
                                // Connection accepted and established
                                // Bind Request-Handler on socket and it fires if data arrive
                                async_read(socket(), boost::asio::buffer(event_head, event_head.size() - 4),
                                           boost::bind(&EventChannelConnection::handle_request, this->shared_from_this(),
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::bytes_transferred));
                            }

                            EventChannelConnection()
                                    : socket_(famouso::util::ios::instance()), incomplete_async_writes(0) {
                            }
                        private:

                            void report(const famouso::mw::Subject &s, const char *const str) {
                                std::cout << "Channel\t\t -- Subject [";
                                for (uint8_t i = 0;i < 8;++i) {
                                    uint8_t c = s.tab()[i];
                                    if ((c < 32) || (c > 126)) c = 32;   // only printable characters
                                    std::cout << c ;
                                }
                                std::cout << "] -> " << str << "\t0x" << std::hex;
                                std::cout.fill('0');
                                std::cout.width(16);
                                std::cout << s.value() << std::endl;
                            }
                            void get_event_head(boost::shared_ptr<PEC> pec,
                                                const boost::system::error_code& error,
                                                size_t bytes_transferred) {
                                if (!error) {
                                    // If it is no publich event, then it have to be an unannouncement.
                                    if (event_head[0] == FAMOUSO::PUBLISH) {
                                        // Allocate self deleting buffer for event_data from EventMemoryPool
                                        uint32_t event_data_size = ntohl(*(uint32_t *) & (event_head[9]));
                                        typename EventMemoryPool::pointer event_data = EventMemoryPool::alloc(event_data_size);
                                        // bind correct handler to the socket in order to
                                        // receive the event_data on that socket
                                        boost::asio::async_read(socket(), boost::asio::buffer(event_data.get(), event_data_size),
                                                                boost::bind(&EventChannelConnection::get_event_data, this->shared_from_this(),
                                                                            pec, event_data, boost::asio::placeholders::error,
                                                                            boost::asio::placeholders::bytes_transferred));
                                        return;
                                    }
                                }
                                report(pec->subject(), "Unannouncement");
                            }

                            void get_event_data(boost::shared_ptr<PEC> pec,
                                                typename EventMemoryPool::pointer event_data,
                                                const boost::system::error_code& error,
                                                size_t bytes_transferred) {
                                if (!error) {
                                    // now the Event is complete
                                    famouso::mw::Event e(pec->subject());
                                    e.length = bytes_transferred;
                                    e.data = (uint8_t *) event_data.get();
                                    current_event_data = event_data;
                                    // publish to FAMOUSO
                                    pec->publish(e);
                                    current_event_data.reset();
                                    // bind correct handler to the socket in order to
                                    // receive a new event_head on that socket
                                    boost::asio::async_read(socket(), boost::asio::buffer(event_head, event_head.size()),
                                                            boost::bind(&EventChannelConnection::get_event_head, this->shared_from_this(),
                                                                        pec, boost::asio::placeholders::error,
                                                                        boost::asio::placeholders::bytes_transferred));
                                    return;
                                }
                                report(pec->subject(), "Unannouncement");
                            }

                            void unsubscribe(boost::shared_ptr<SEC> sec,
                                             const boost::system::error_code& error) {
                                report(sec->subject(), "Unsubscription");
                            }

                            void write_handler(typename EventMemoryPool::pointer event_data,
                                               typename EventMemoryPool::pointer event_preamble,
                                               const boost::system::error_code & error,
                                               size_t bytes_transferred) {
                                incomplete_async_writes--;
                            }

                            void cb(famouso::mw::api::SECCallBackData & cbd) {
                                if (incomplete_async_writes < 5) {
                                    // Idea: performance improvement for multiple subscribers: alloc and init preamble once in get_event_data() like event_data
                                    typename EventMemoryPool::pointer sp_preamble = EventMemoryPool::alloc(13);
                                    uint8_t * preamble = sp_preamble.get();
                                    preamble[0] = FAMOUSO::PUBLISH;
                                    uint32_t *len = (uint32_t *) & preamble[9];
                                    for (uint8_t i = 0;i < 8;++i)
                                        preamble[i+1] = cbd.subject.tab()[i];
                                    *len = htonl(cbd.length);
                                    boost::array<boost::asio::const_buffer, 2> bufs = {{
                                            boost::asio::buffer(preamble, 13),
                                            boost::asio::buffer(cbd.data, cbd.length)
                                        }
                                    };
                                    incomplete_async_writes++;
                                    // bufs memory owned by shared_ptr current_event_data and sp_preamble passed
                                    // to write_handler()
                                    boost::asio::async_write(socket(), bufs, boost::asio::transfer_all(),
                                                             boost::bind(&EventChannelConnection::write_handler, this->shared_from_this(),
                                                                         current_event_data,
                                                                         sp_preamble,
                                                                         boost::asio::placeholders::error,
                                                                         boost::asio::placeholders::bytes_transferred));
                                } else {
                                    // Too many incomplete writes... assume that the client is crashed or
                                    // blocking inside the callback. Drop current event to prevent flooding
                                    // memory with async write operations which are likely to be never
                                    // completed.
                                }
                            }

                            void handle_request(const boost::system::error_code& error, size_t bytes_transferred) {
                                if (!error && (bytes_transferred >= 9)) {
                                    switch (event_head[0]) {
                                        case FAMOUSO::SUBSCRIBE: {
                                                // allocate a new subscribe event channel
                                                boost::shared_ptr< SEC > sec(new SEC(famouso::mw::Subject(&event_head[1])));
                                                // announce it to FAMOUSO
                                                sec->subscribe();
                                                // set a specific callback
                                                sec->callback.template bind< EventChannelConnection, &EventChannelConnection::cb >(this);

                                                // bind the receive function, however an unsubscription can come only
                                                boost::asio::async_read(socket(), boost::asio::buffer(event_head, event_head.size()),
                                                                        boost::bind(&EventChannelConnection::unsubscribe, this->shared_from_this(),
                                                                                    sec, boost::asio::placeholders::error));

                                                report(sec->subject(), "Subscription  ");
                                                break;
                                            }
                                        case FAMOUSO::ANNOUNCE: {
                                                // allocate a new publish event channel
                                                boost::shared_ptr<PEC> pec(new PEC(famouso::mw::Subject(&event_head[1])));
                                                // announce it to FAMOUSO
                                                pec->announce();

                                                // bind the receive function for published events or for the unannouncement event
                                                boost::asio::async_read(socket(), boost::asio::buffer(event_head, event_head.size()),
                                                                        boost::bind(&EventChannelConnection::get_event_head, this->shared_from_this(),
                                                                                    pec, boost::asio::placeholders::error,
                                                                                    boost::asio::placeholders::bytes_transferred));

                                                report(pec->subject(), "Announcement  ");
                                                break;
                                            }
                                        default:
                                            std::cerr << "Wrong opcode:\t0x" << event_head[0] << std::endl;
                                    }
                                } else {
                                    std::cerr << "Wrong message format:" << std::endl;
                                }
                            }

                            boost::asio::ip::tcp::socket socket_;

                            // Number of asynchronous writes to the client not yet completed
                            unsigned int incomplete_async_writes;

                            // Buffer used to store data received from the client.
                            boost::array<uint8_t, 13> event_head;

                            // Workaround for transporting shared_ptr from get_event_data() into cb()
                            typename EventMemoryPool::pointer current_event_data;

                    };


                public:
                    EventLayerMiddlewareStub() : acceptor_(famouso::util::ios::instance(),
                                                                   boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
                                                                                                  ServPort)) {
                        start_accept();
                        // start io-service to ensure asynchrone acceptance of connections
                        famouso::util::impl::start_ios();
                    }

                private:
                    void start_accept() {
                        typename EventChannelConnection<EL>::pointer ecc = EventChannelConnection<EL>::create();
                        acceptor_.async_accept(ecc->socket(),
                                               boost::bind(&EventLayerMiddlewareStub::handle_accept, this, ecc,
                                                           boost::asio::placeholders::error));
                    }

                    void handle_accept(typename EventChannelConnection<EL>::pointer ecc,
                                       const boost::system::error_code& error) {
                        if (!error) {
                            ecc->start();
                            start_accept();
                        } else {
                            std::cerr << "Error in asynchronous acceptance of an incoming connection or CTRL^C" << std::endl;
                        }
                    }
                    boost::asio::ip::tcp::acceptor acceptor_;
            };

        } // namespace el
    } // namespace mw
} // namespace famouso

#endif /* __EventLayerMiddlewareStub_h__ */
