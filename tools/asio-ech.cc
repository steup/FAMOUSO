#include <ctime>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <iostream>
#include <list>

#include <stdio.h>
#include <stdint.h>

#include "mw/nl/UDPBroadCastNL.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/common/Event.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

//#include "util/TCPSocketThread.h"
//#include "devices/nic/ip/socket/TCPServerSocket.h"
#include "devices/nic/ip/socket/TCPSocket.h"

#include "famouso.h"
#include "util/endianess.h"
#include "util/ios.h"

namespace famouso {

    typedef famouso::mw::nl::UDPBroadCastNL	nl;
    typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
    typedef famouso::mw::el::EventLayer< anl > el;
    typedef famouso::mw::api::EventChannel< el > EC;
    typedef famouso::mw::api::PublisherEventChannel<EC> PEC;
    typedef famouso::mw::api::SubscriberEventChannel<EC> SEC;


class EventChannelConnection : public boost::enable_shared_from_this<EventChannelConnection> {
public:
    typedef boost::shared_ptr<EventChannelConnection> pointer;

    static pointer create() {
        return pointer(new EventChannelConnection());
    }

    asio::ip::tcp::socket& socket() {
        return socket_;
    }

    void start() {
        // Connection accepted and established
        // Bind Request-Handler on socket and it fires if data arrive
        async_read(socket(),asio::buffer(event_head, event_head.size()-4),
                   boost::bind(&EventChannelConnection::handle_request, shared_from_this(),
                               asio::placeholders::error,
                               asio::placeholders::bytes_transferred));
    }

private:
    EventChannelConnection()
            : socket_(famouso::ios::instance()) {
    }

    void get_event_head(boost::shared_ptr<PEC> pec,
                        const asio::error_code& error,
                        size_t bytes_transferred) {
        if (!error) {
            // If it is no publich event, then it have to be an unannouncement.
            if (event_head[0]==FAMOUSO::PUBLISH) {
                // bind correct handler to the socket in order to
                // receive the event_data on that socket
                asio::async_read(socket(), asio::buffer(event_data, ntohl (*(uint32_t *)&(event_head[9]))),
                                 boost::bind(&EventChannelConnection::get_event_data, shared_from_this(),
                                             pec, asio::placeholders::error,
                                             asio::placeholders::bytes_transferred));
                return;
            }
        }
        std::cout << "Unannouncement for channel:\t0x" << std::hex << pec->subject().value << std::endl;
    }

    void get_event_data(boost::shared_ptr<PEC> pec,
                        const asio::error_code& error,
                        size_t bytes_transferred) {
        if (!error) {
            // now the Event is complete
            famouso::mw::Event e(pec->subject());
            e.length=bytes_transferred;
            e.data = (uint8_t *) &event_data;
            // publish to FAMOUSO
            pec->publish(e);
            // bind correct handler to the socket in order to
            // receive a new event_head on that socket
            asio::async_read(socket(), asio::buffer(event_head, event_head.size()),
                             boost::bind(&EventChannelConnection::get_event_head, shared_from_this(),
                                         pec, asio::placeholders::error,
                                         asio::placeholders::bytes_transferred));
            return;
        }
        std::cout << "Unannouncement for channel:\t0x" << std::hex << pec->subject().value << std::endl;
    }

    void unsubscribe(boost::shared_ptr<SEC> sec,
                     const asio::error_code& error) {
        std::cout << "Unsubscription for channel:\t0x"
        << std::hex << sec->subject().value
        << std::endl;
    }

    void cb (EC::CallBackData & cbd) {
        uint8_t preamble[13]= {FAMOUSO::PUBLISH};
        uint64_t *sub = (uint64_t *) & preamble[1];
        uint32_t *len = (uint32_t *) & preamble[9];
        *sub = htonll(cbd.subject.value);
        *len = htonl(cbd.length);
        asio::write(socket(), asio::buffer(preamble, 13));
        asio::write(socket(), asio::buffer(cbd.data, cbd.length));
    }

    void handle_request(const asio::error_code& error, size_t bytes_transferred) {
        if (!error && (bytes_transferred >=9) ) {
            switch (event_head[0]) {
            case FAMOUSO::SUBSCRIBE: {
                // allocate a new subscribe event channel
                boost::shared_ptr<SEC> sec( new SEC(ntohll(*(uint64_t *) &(event_head[1]))));
                // announce it to FAMOUSO
                sec->subscribe ();
                // set a specific callback
                sec->callback.from_function < EventChannelConnection, &EventChannelConnection::cb > (this);

                // bind the receive function, however an unsubscription can come only
                asio::async_read(socket(), asio::buffer(event_head, event_head.size()),
                                 boost::bind(&EventChannelConnection::unsubscribe, shared_from_this(),
                                             sec, asio::placeholders::error));

                std::cout << "Subscription for channel:\t0x" << std::hex << sec->subject().value << std::endl;
                break;
            }
            case FAMOUSO::ANNOUNCE: {
                // allocate a new publish event channel
                boost::shared_ptr<PEC> pec( new PEC(ntohll(*(uint64_t *) &(event_head[1]))));
                // announce it to FAMOUSO
                pec->announce ();

                // bind the receive function for published events or for the unannouncement event
                asio::async_read(socket(), asio::buffer(event_head, event_head.size()),
                                 boost::bind(&EventChannelConnection::get_event_head, shared_from_this(),
                                             pec, asio::placeholders::error,
                                             asio::placeholders::bytes_transferred));

                std::cout << "Announcement for channel:\t0x" << std::hex << pec->subject().value << std::endl;
                break;
            }
	    default:
                std::cout << "Wrong opcode:\t0x" << event_head[0] << std::endl;
            }
        } else {
                std::cout << "Wrong message format:" << std::endl;
	}
    }

    asio::ip::tcp::socket socket_;
    // Buffer used to store data received from the client.
    boost::array<char, 13> event_head;
    boost::array<char, 65535> event_data;

};


class EventChannelHandler {
public:
    EventChannelHandler() : acceptor_(famouso::ios::instance(),
				      asio::ip::tcp::endpoint(asio::ip::tcp::v4(),
							      ServPort)) {
	start_accept();
    }

private:
    void start_accept() {
        EventChannelConnection::pointer ecc = EventChannelConnection::create();
        acceptor_.async_accept(ecc->socket(),
                               boost::bind(&EventChannelHandler::handle_accept, this, ecc,
                                           asio::placeholders::error));
    }

    void handle_accept(EventChannelConnection::pointer ecc,
                       const asio::error_code& error) {
        if (!error) {
            ecc->start();
            start_accept();
        }
    }

    asio::ip::tcp::acceptor acceptor_;
};

}


void do_post(){
	std::cout<<"\tMichael"<<std::endl;
}


int main (int argc, char **argv) {
    std::cout << "Project: FAMOUSO" << std::endl;
    std::cout << "local Event Channel Handler" << std::endl << std::endl;
    std::cout << "Author: Michael Schulze" << std::endl;
    std::cout << "Revision: $Rev$" << std::endl;
    std::cout << "$Date$" << std::endl;
    std::cout << "build at " << __TIME__ << std::endl;
    std::cout << "last changed by $Author$" << std::endl << std::endl;

    try {
	famouso::EventChannelHandler localECH;
	famouso::ios::instance().run();

//	asio::thread t(boost::bind(&famouso::ios_type::run, &famouso::ios::instance()));
//	while(1){
//	    sleep(1);
//	    famouso::ios::instance().post(boost::bind(&do_post));
//	}
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
