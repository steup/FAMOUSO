/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                         Philipp Werner <philipp.werner@st.ovgu.de>
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

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/pool/pool.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/placeholders.hpp>
#include <string.h>
#include <queue>

#include "debug.h"

#include "mw/common/Event.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "mw/el/LocalConnectionParameters.h"
#include "mw/afp/shared/Time.h"
#include "util/endianness.h"
#include "util/ios.h"

namespace famouso {
    namespace mw {
        namespace el {

            /*!
             *  \brief Event layer stub that allows running multiple famouso apps
             *         on one computer.
             *
             *  This object accepts TCP connections from other apps which use EventLayerClientStub
             *  as event layer and distributes events among all subscribers on this computer and
             *  in the network accessible via network layer.
             */
            template <typename EL>
            class EventLayerMiddlewareStub {

                    /*!
                     *  \brief Pool memory allocator returning shared_ptr deleting themself. Not thread-safe ATM!
                     */
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
                            /// Self-deleting pointer
                            typedef boost::shared_ptr<uint8_t> pointer;

                            /*!
                             *  \brief Allocate self-deleting (reference counting) buffer
                             *  \param size  Size of the buffer in bytes. 0 < size <= 64k
                             */
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
                                    BOOST_ASSERT(size <= 64*1024);
                                    pointer p((uint8_t *)event_data_64k().malloc(), event_data_64k_deleter());
                                    return p;
                                }
                            }
                            // maybe call release_memory() sometimes
                    };


                    /*!
                     *  \brief TCP connection to an event channel of another app on this computer
                     */
                    template <typename lECH>
                    class EventChannelConnection : public boost::enable_shared_from_this< EventChannelConnection<lECH> > {
                            typedef famouso::mw::api::PublisherEventChannel<lECH> PEC;
                            typedef famouso::mw::api::SubscriberEventChannel<lECH> SEC;

                            /// Pointer to data buffer from EventMemoryPool (reference counted with automatic deletion)
                            typedef typename EventMemoryPool::pointer data_ptr;


                            /// Type for queueing of asynchronous write requests
                            struct AsyncWriteRequest {

                                AsyncWriteRequest(const data_ptr & preamble, const data_ptr & event_data, uint32_t event_length) :
                                    preamble(preamble), event_data(event_data), event_length(event_length) {
                                }

                                AsyncWriteRequest(const AsyncWriteRequest & awr) {
                                    this->preamble = awr.preamble;
                                    this->event_data = awr.event_data;
                                    this->event_length = awr.event_length;
                                }

                                data_ptr preamble;
                                data_ptr event_data;
                                uint32_t event_length;
                            };


                            /// Workaround for transporting shared_ptr from get_event_data() into cb()
                            static data_ptr & current_event_data() {
                                static data_ptr data;
                                return data;
                            }

                            /// Workaround for transporting shared_ptr from get_event_data() into cb()
                            static data_ptr & current_preamble() {
                                static data_ptr data;
                                return data;
                            }

                        public:
                            /*!
                             *  \brief Use shared pointers (from boost) for automated deletion
                             */
                            typedef boost::shared_ptr<EventChannelConnection> pointer;

                            /*!
                             *  \brief Create new EventChannelConnection and return pointer
                             */
                            static pointer create() {
                                return pointer(new EventChannelConnection());
                            }

                            /*!
                             * \brief Returns socket used to communicate with the client
                             */
                            boost::asio::ip::tcp::socket& socket() {
                                return socket_;
                            }

                            /*!
                             *  \brief Start communication with client
                             */
                            void start() {
                                // Connection accepted and established
                                // Bind Request-Handler on socket and it fires if data arrive
                                async_read(socket(), boost::asio::buffer(event_head, event_head.size() - 4),
                                           boost::bind(&EventChannelConnection::handle_request, this->shared_from_this(),
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::bytes_transferred));
                            }

                        private:
                            /*!
                             *  \brief Constructs EventChannelConnection. Use create to get a new instance.
                             */
                            EventChannelConnection()
                                    : socket_(famouso::util::ios::instance()), incomplete_async_write_bytes(0),
                                      incomplete_async_writes(0) {
                            }

                            /*!
                             *  \brief Write something about a channel into the log.
                             *  \param s   Subject of the channel
                             *  \param str Message
                             */
                            void report(const famouso::mw::Subject &s, const char *const str) {

                                ::logging::log::emit() << str << ::logging::log::tab
                                    << ::logging::log::tab << "-- Subject [";
                                for (uint8_t i = 0;i < 8;++i) {
                                    uint8_t c = s.tab()[i];
                                    if ((c < 32) || (c > 126)) c = 32;   // only printable characters
                                    ::logging::log::emit() << c ;
                                }
                                ::logging::log::emit() << "] -> " << ::logging::log::hex
                                    << s.value() << ::logging::log::endl;
                            }

                            /*!
                             *  \brief Async read handler for event head (publisher connection)
                             *  \param pec               Publisher event channel
                             *  \param error             Error
                             *  \param bytes_transferred Number of bytes transferred
                             */
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

                            /*!
                             *  \brief Async read handler for event data (publisher connection)
                             *  \param pec               Publisher event channel
                             *  \param event_data        Buffer (self-deleting) containing event data read
                             *  \param error             Error
                             *  \param bytes_transferred Number of bytes transferred
                             */
                            void get_event_data(boost::shared_ptr<PEC> pec,
                                                typename EventMemoryPool::pointer event_data,
                                                const boost::system::error_code& error,
                                                size_t bytes_transferred) {
                                if (!error) {
                                    // now the Event is complete
                                    famouso::mw::Event e(pec->subject());
                                    e.length = bytes_transferred;
                                    e.data = (uint8_t *) event_data.get();
                                    // publish to FAMOUSO
                                    // Use static members to transport the shared pointers to cb().
                                    current_event_data() = event_data;
                                    current_preamble() = create_publish_preamble(e.subject, e.length);
                                    pec->publish(e);
                                    current_event_data().reset();
                                    current_preamble().reset();
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

                            /*!
                             *  \brief Async read handler (subscriber connections)
                             *  \param sec   Subscriber event channel
                             *  \param error Error
                             *
                             *  Data received from subscriber connection can only be an unsubscription.
                             */
                            void unsubscribe(boost::shared_ptr<SEC> sec,
                                             const boost::system::error_code& error) {
                                report(sec->subject(), "Unsubscription");
                            }

                            /*!
                             *  \brief Async write completion handler (subscriber connections)
                             *  \param event_preamble    Event preamble written
                             *  \param event_data        Event data written
                             *  \param error             Error
                             *  \param bytes_transferred Number of bytes transferred
                             *
                             *  After leaving this function reference counters of event_data and event_preamble will
                             *  reach zero and buffers will be freed.
                             */
                            void write_handler(data_ptr event_preamble,
                                               data_ptr event_data,
                                               const boost::system::error_code & error,
                                               size_t bytes_transferred) {
                                // Update counters
                                incomplete_async_write_bytes -= bytes_transferred;
                                incomplete_async_writes--;

                                // Post next async write operation if there is one
                                if (!async_write_requests.empty()) {
                                    AsyncWriteRequest & awr = async_write_requests.front();
                                    post_async_write(awr.preamble, awr.event_data, awr.event_length);
                                    async_write_requests.pop();
                                }
                            }

                            /*!
                             *  \brief Posts async write operation to be performed next (subscriber connections)
                             *  \param event_preamble    Event preamble to be written
                             *  \param event_data        Event data to be written
                             *  \param event_length      Length of event data
                             *
                             *  After completion of the async write operation write_hanlder() will be called.
                             */
                            void post_async_write(data_ptr event_preamble, data_ptr event_data, uint32_t event_length) {
                                boost::array<boost::asio::const_buffer, 2> bufs = {{
                                    boost::asio::buffer(event_preamble.get(), 13),
                                    boost::asio::buffer(event_data.get(), event_length)
                                }};
                                boost::asio::async_write(socket(), bufs, boost::asio::transfer_all(),
                                                         boost::bind(&EventChannelConnection::write_handler, this->shared_from_this(),
                                                                     event_preamble,
                                                                     event_data,
                                                                     boost::asio::placeholders::error,
                                                                     boost::asio::placeholders::bytes_transferred));
                            }

                            /*!
                             *  \brief Create event publish preamble (subscriber connections)
                             *  \param subject          Subject of the channel of this subscriber
                             *  \param event_length     Subject of the channel of this subscriber
                             */
                            data_ptr create_publish_preamble(const famouso::mw::Subject & subject, uint32_t event_length) {
                                data_ptr sp_preamble = EventMemoryPool::alloc(13);
                                uint8_t * preamble = sp_preamble.get();
                                preamble[0] = FAMOUSO::PUBLISH;
                                uint32_t *len = (uint32_t *) & preamble[9];
                                for (uint8_t i = 0;i < 8;++i)
                                    preamble[i+1] = subject.tab()[i];
                                *len = htonl(event_length);
                                return sp_preamble;
                            }

                            /*!
                             *  \brief Subscriber channel callback that forwards events to client app (subscriber connections)
                             *  \param cbd  Callback data (event to forward to this subscriber)
                             *  \todo   Remove copying of events coming from network layer (with buffers provided by application layer!?)
                             */
                            void cb(famouso::mw::api::SECCallBackData & cbd) {
                                // Crash detection and flow control:
                                // If data is published in a higher rate than the subscriber
                                // can process it, the async write operation queue will flood
                                // the memory. To avoid dropping data in case of high peak throughput
                                // nothing is discarded before a deadline was passed. The deadline
                                // is given relative to the last time no async write was pending.
                                if (!incomplete_async_writes) {
                                    // All async writes complete -> reset deadline
                                    drop_deadline = Time::current().add_sec(1);
                                }

                                if (incomplete_async_write_bytes < 100000 || Time::current() < drop_deadline) {
                                    // To enable asynchronous write operations (needed to avoid flow dependencies)
                                    // we use buffers that are managed by shared_ptr pointers and are deleted
                                    // automatically after async write is completed.
                                    data_ptr sp_preamble;
                                    data_ptr sp_event_data;

                                    if (current_event_data()) {
                                        // Event published by get_event_data() (published locally).
                                        // -> cbd.data owned by current_event_data()
                                        sp_event_data = current_event_data();
                                        sp_preamble = current_preamble();
                                    } else {
                                        // Event comes from network layer
                                        // -> copy data to enable async write
                                        sp_event_data = EventMemoryPool::alloc(cbd.length);
                                        memcpy(sp_event_data.get(), cbd.data, cbd.length);
                                        sp_preamble = create_publish_preamble(cbd.subject, cbd.length);
                                    }

                                    // Update counters
                                    incomplete_async_write_bytes += cbd.length + 13;
                                    incomplete_async_writes++;

                                    if (incomplete_async_writes == 1) {
                                        // No other async writes pending
                                        // -> post it directly
                                        FAMOUSO_ASSERT(async_write_requests.empty());
                                        post_async_write(sp_preamble, sp_event_data, cbd.length);
                                    } else {
                                        // Other async writes pending
                                        // -> asio does not support queueing of async_write() calls (would result in interleaving)
                                        // -> queueing in application
                                        async_write_requests.push(AsyncWriteRequest(sp_preamble, sp_event_data, cbd.length));
                                    }
                                } else {
                                    // The client is crashed, blocking inside the callback or just to
                                    // slow while processing the data. Drop current event to prevent flooding
                                    // memory with async write operations which are likely to be never
                                    // completed.
                                }
                            }

                            /*!
                             *  \brief Async read handler for client's request (first data sent to define whether this
                             *	       connection is used for subscriber of publisher channel)
                             *  \param error             Async read error information
                             *  \param bytes_transferred Number of bytes read
                             */
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
                                            ::logging::log::emit< ::logging::Error>() << "Wrong opcode:\t0x"
                                                << event_head[0] << ::logging::log::endl;
                                    }
                                } else {
                                    ::logging::log::emit< ::logging::Error>() << "Wrong message format:"
                                        << ::logging::log::endl;
                                }
                            }

                            /// TCP socket used for communication with the client's event layer stub
                            boost::asio::ip::tcp::socket socket_;

                            /// Amount of data pending for asynchronous writes to the client subscriber
                            unsigned int incomplete_async_write_bytes;

                            /// Count of incomplete asynchronous write requests to the client subscriber
                            unsigned int incomplete_async_writes;

                            /// After this time no async write requests get queued anymore (subscriber crash detection and flow control)
                            Time drop_deadline;

                            /// Queue of async write requests to the subscribing client not yet posted
                            std::queue<AsyncWriteRequest> async_write_requests;

                            /// Buffer used to store data received from the client
                            boost::array<uint8_t, 13> event_head;
                    };


                public:
                    /*!
                     *  \brief Construct EventLayerMiddlewareStub and start accepting incoming connections
                     */
                    EventLayerMiddlewareStub() : acceptor_(famouso::util::ios::instance(),
                                                                   boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
                                                                                                  ServPort)) {
                        start_accept();
                        // start io-service to ensure asynchrone acceptance of connections
                        famouso::util::impl::start_ios();
                    }

                private:
                    /*!
                     *  \brief Start accepting incoming connections
                     */
                    void start_accept() {
                        typename EventChannelConnection<EL>::pointer ecc = EventChannelConnection<EL>::create();
                        acceptor_.async_accept(ecc->socket(),
                                               boost::bind(&EventLayerMiddlewareStub::handle_accept, this, ecc,
                                                           boost::asio::placeholders::error));
                    }

                    /*!
                     *  \brief Async accept handler
                     *  \param ecc   New event channel connection
                     *  \param error Error
                     */
                    void handle_accept(typename EventChannelConnection<EL>::pointer ecc,
                                       const boost::system::error_code& error) {
                        if (!error) {
                            ecc->start();
                            start_accept();
                        } else {
                            ::logging::log::emit< ::logging::Error>()
                                << "Error in asynchronous acceptance of an incoming connection or CTRL^C"
                                << ::logging::log::endl;
                        }
                    }
                    boost::asio::ip::tcp::acceptor acceptor_;
            };

        } // namespace el
    } // namespace mw
} // namespace famouso

#endif /* __EventLayerMiddlewareStub_h__ */
