#ifndef __EventLayerMiddlewareStub_h__
#define __EventLayerMiddlewareStub_h__
//
//#include <ctime>
//#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
//#include <boost/thread/xtime.hpp>

#include <iostream>
//#include <list>

#include "mw/common/Event.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "famouso.h"
#include "util/endianess.h"
#include "util/ios.h"

//#include "mw/common/UID.h"


namespace famouso {
namespace mw {
namespace el {

    template <typename EL>
    class EventLayerMiddlewareStub {

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
                    : socket_(famouso::util::ios::instance()) {
            }
        private:

            void get_event_head(boost::shared_ptr<PEC> pec,
                                const boost::system::error_code& error,
                                size_t bytes_transferred) {
                if (!error) {
                    // If it is no publich event, then it have to be an unannouncement.
                    if (event_head[0] == FAMOUSO::PUBLISH) {
                        // bind correct handler to the socket in order to
                        // receive the event_data on that socket
                        boost::asio::async_read(socket(), boost::asio::buffer(event_data, ntohl (*(uint32_t *)&(event_head[9]))),
                                                boost::bind(&EventChannelConnection::get_event_data, this->shared_from_this(),
                                                            pec, boost::asio::placeholders::error,
                                                            boost::asio::placeholders::bytes_transferred));
                        return;
                    }
                }
                std::cout << "Channel\t\t -- Subject [0x" << std::hex
                << pec->subject().value << "] -> Unannouncement"
                << std::endl;
            }

            void get_event_data(boost::shared_ptr<PEC> pec,
                                const boost::system::error_code& error,
                                size_t bytes_transferred) {
                if (!error) {
                    // now the Event is complete
                    famouso::mw::Event e(pec->subject());
                    e.length = bytes_transferred;
                    e.data = (uint8_t *) & event_data;
                    // publish to FAMOUSO
                    pec->publish(e);
                    // bind correct handler to the socket in order to
                    // receive a new event_head on that socket
                    boost::asio::async_read(socket(), boost::asio::buffer(event_head, event_head.size()),
                                            boost::bind(&EventChannelConnection::get_event_head, this->shared_from_this(),
                                                        pec, boost::asio::placeholders::error,
                                                        boost::asio::placeholders::bytes_transferred));
                    return;
                }
                std::cout << "Channel\t\t -- Subject [0x" << std::hex
                << pec->subject().value << "] -> Unannouncement"
                << std::endl;
            }

            void unsubscribe(boost::shared_ptr<SEC> sec,
                             const boost::system::error_code& error) {
                std::cout << "Channel\t\t -- Subject [0x" << std::hex
                << sec->subject().value << "] -> Unsubscription"
                << std::endl;
            }

            void cb (famouso::mw::api::SECCallBackData & cbd) {
                uint8_t preamble[13] = {FAMOUSO::PUBLISH};
                uint64_t *sub = (uint64_t *) & preamble[1];
                uint32_t *len = (uint32_t *) & preamble[9];
                *sub = htonll(cbd.subject.value);
                *len = htonl(cbd.length);
                boost::asio::write(socket(), boost::asio::buffer(preamble, 13),
                                   boost::asio::transfer_all());
                boost::asio::write(socket(), boost::asio::buffer(cbd.data, cbd.length),
                                   boost::asio::transfer_all());
            }

            void handle_request(const boost::system::error_code& error, size_t bytes_transferred) {
                if (!error && (bytes_transferred >= 9) ) {
                    switch (event_head[0]) {
                        case FAMOUSO::SUBSCRIBE: {
                                // allocate a new subscribe event channel
                                boost::shared_ptr< SEC > sec( new SEC(ntohll(*(uint64_t *) &(event_head[1]))));
                                // announce it to FAMOUSO
                                sec->subscribe ();
                                // set a specific callback
                                sec->callback.template bind< EventChannelConnection, &EventChannelConnection::cb >(this);

                                // bind the receive function, however an unsubscription can come only
                                boost::asio::async_read(socket(), boost::asio::buffer(event_head, event_head.size()),
                                                        boost::bind(&EventChannelConnection::unsubscribe, this->shared_from_this(),
                                                                    sec, boost::asio::placeholders::error));

                                std::cout << "Channel\t\t -- Subject [0x" << std::hex
                                << sec->subject().value << "] -> Subscription"
                                << std::endl;
                                break;
                            }
                        case FAMOUSO::ANNOUNCE: {
                                // allocate a new publish event channel
                                boost::shared_ptr<PEC> pec( new PEC(ntohll(*(uint64_t *) &(event_head[1]))));
                                // announce it to FAMOUSO
                                pec->announce ();

                                // bind the receive function for published events or for the unannouncement event
                                boost::asio::async_read(socket(), boost::asio::buffer(event_head, event_head.size()),
                                                        boost::bind(&EventChannelConnection::get_event_head, this->shared_from_this(),
                                                                    pec, boost::asio::placeholders::error,
                                                                    boost::asio::placeholders::bytes_transferred));

                                std::cout << "Channel\t\t -- Subject [0x" << std::hex
                                << pec->subject().value << "] -> Announcement"
                                << std::endl;
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
            // Buffer used to store data received from the client.
            boost::array<char, 13> event_head;
            boost::array<char, 65535> event_data;

    };


        public:
            EventLayerMiddlewareStub() : acceptor_(famouso::util::ios::instance(),
                                                      boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
                                                                                     ServPort)) {
                start_accept();
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
                }
            }
            boost::asio::ip::tcp::acceptor acceptor_;
    };

} // namespace el
} // namespace mw
} // namespace famouso

#endif /* __EventLayerMiddlewareStub_h__ */