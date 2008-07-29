#include <ctime>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <asio.hpp>

#include <iostream>
#include <list>

#include <stdio.h>
#include <stdint.h>

#include "mw/nl/voidNL.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/common/Event.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "util/TCPSocketThread.h"
#include "devices/nic/ip/socket/TCPServerSocket.h"
#include "devices/nic/ip/socket/TCPSocket.h"

#include "famouso.h"
#include "util/endianess.h"

typedef EventChannel<EventLayer<AbstractNetworkLayer<voidNL> > > EC;
typedef PublisherEventChannel<EC> PEC;
typedef SubscriberEventChannel<EC> SEC;

using asio::ip::tcp;

class EventChannelConnection : public boost::enable_shared_from_this<EventChannelConnection> {
public:
  typedef boost::shared_ptr<EventChannelConnection> pointer;

  static pointer create(asio::io_service& io_service) {
    return pointer(new EventChannelConnection(io_service));
  }

  tcp::socket& socket() {
    return socket_;
  }

  void start() {
    std::cout<<"Connection accepted and established"<<endl;
    async_read(socket(),asio::buffer(event_head, event_head.size()-4),
	       boost::bind(&EventChannelConnection::handle_request, shared_from_this(),
			   asio::placeholders::error,
			   asio::placeholders::bytes_transferred));
  }

  ~EventChannelConnection(){
    std::cout<<"EventChannelConnection::~EventChannelConnection()"<<std::endl;
  }
private:

  EventChannelConnection(asio::io_service& io_service)
    : socket_(io_service) {
  }

  void handle_write(const asio::error_code& /*error*/,
		    size_t /*bytes_transferred*/) {
  }

  void get_event_head(boost::shared_ptr<PEC> pec,
		      const asio::error_code& error,
		      size_t bytes_transferred)
  {
    //    Event e(pec->subject());
    if (!error){
      if (event_head[0]==FAMOUSO::PUBLISH){
	if (bytes_transferred<13)
	  asio::read(socket(), asio::buffer(&event_head[bytes_transferred], 13-bytes_transferred));

	async_read(socket(), asio::buffer(event_data, ntohl (*(uint32_t *)&(event_head[9]))),
		   boost::bind(&EventChannelConnection::get_event_data, shared_from_this(),
			       pec, asio::placeholders::error,
			       asio::placeholders::bytes_transferred));
	return;
      }
    }
    std::cout << "Unannouncement for channel:\t0x" << hex << pec->subject().value << std::endl;
    socket().close();
    
  }
  
  void get_event_data(boost::shared_ptr<PEC> pec,
		      const asio::error_code& error,
		      size_t bytes_transferred)
  {
    if (!error){
      cout<<"Event vollstaendig erhalten"<<std::endl;
      Event e(pec->subject());
      e.length=bytes_transferred;
      e.data = (uint8_t *) &event_data;
      pec->publish(e);
      socket().async_read_some(asio::buffer(event_head, event_head.size()),
			       boost::bind(&EventChannelConnection::get_event_head, shared_from_this(),
					   pec, asio::placeholders::error,
					   asio::placeholders::bytes_transferred));
      return;
    }
    std::cout << "Unannouncement for channel:\t0x" << hex << pec->subject().value << std::endl;
    socket().close();
  }

  void unsubscribe(boost::shared_ptr<SEC> sec, 
		   const asio::error_code& error)
  {
      std::cout << "Unannouncement for channel:\t0x" 
		<< hex << sec->subject().value 
		<< std::endl;
      socket().close();
  }

  void cb (EC::CallBackData & cbd) {
     uint8_t preamble[13]= {FAMOUSO::PUBLISH};
     uint64_t *sub = (uint64_t *) & preamble[1];
     uint32_t *len = (uint32_t *) & preamble[9];
     *sub = htonll(cbd.subject.value);
     *len = htonl(cbd.length);
     asio::write(socket(), asio::buffer(preamble, 13));
     asio::write(socket(), asio::buffer(cbd.data, cbd.length));
     std::cout<<"callback called" <<std::endl;
  }

  void handle_request(const asio::error_code& error, size_t bytes_transferred) {
    std::cout<<"request"<<endl;
    if (!error){
      switch (event_head[0]) {
      case FAMOUSO::SUBSCRIBE: {
	// ein Interesse an einem Subject wird angekuendigt
	//
	/// \todo wenn eine Anwendung einen Kanal mehrfach subscribiert, werden mehrere Kanaele angelegt, aber dies ist halt so
	/// \todo kann man aber auch abfangen und sollte man auch noch machen
	boost::shared_ptr<SEC> sec( new SEC(ntohll(*(uint64_t *) &(event_head[1]))));
	// Callback eintragen
	// und ankuendigen
	sec->subscribe ();
	std::cout << "Subscription for channel:\t0x" << hex << sec->subject().value << std::endl;
	sec->callback.from_function < EventChannelConnection, &EventChannelConnection::cb > (this);

	// trage Empfangsfunktion ein, aber es kann eigentlich nur die Abmeldung kommen
	socket().async_read_some(asio::buffer(event_head, event_head.size()),
				 boost::bind(&EventChannelConnection::unsubscribe, shared_from_this(),
					     sec, asio::placeholders::error));
	break;
      }
      case FAMOUSO::ANNOUNCE: {
	// Hier muss ein neuer Kanal erstellt werden, falls er noch nicht existiert
	boost::shared_ptr<PEC> pec( new PEC(ntohll(*(uint64_t *) &(event_head[1]))));
	// Callback eintragen
	// und ankuendigen
	pec->announce ();
	std::cout << "Announcement for channel:\t0x" << hex << pec->subject().value << std::endl;

	// trage Empfangsfunktion fuer published events ein, aber es kann auch die Abmeldung sein
	socket().async_read_some(asio::buffer(event_head, event_head.size()),
				 boost::bind(&EventChannelConnection::get_event_head, shared_from_this(),
					     pec, asio::placeholders::error,
					     asio::placeholders::bytes_transferred));

	break;
      }
      default:
	socket().close();
	std::cout << "wrong format connection closed or refused" << std::endl;
      }
//       socket().async_read_some(asio::buffer(event_head, event_head.size()),
// 			       boost::bind(&EventChannelConnection::handle_request, shared_from_this(),
// 					   asio::placeholders::error,
// 					   asio::placeholders::bytes_transferred));
    } else {
      std::cout<<"error occured"<<endl;
      socket().close();
    }
  }

  tcp::socket socket_;
  // Buffer used to store data received from the client.
  boost::array<char, 13> event_head;
  boost::array<char, 65535> event_data;

};

class EventChannelHandler
{
public:
  EventChannelHandler(asio::io_service& io_service) : acceptor_(io_service, tcp::endpoint(tcp::v4(), ServPort)) {
    start_accept();
  }

private:
  void start_accept() {
    EventChannelConnection::pointer ecc = EventChannelConnection::create(acceptor_.io_service());

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

  tcp::acceptor acceptor_;
};

int main (int argc, char **argv)
{
  cout << "Project: FAMOUSO" << endl;
  cout << "local Event Channel Handler" << endl << endl;
  cout << "Author: Michael Schulze" << endl;
  cout << "Revision: $Rev: 341 $" << endl;
  cout << "$Date: 2008-05-19 11:30:29 +0200 (Mon, 19 May 2008) $" <<endl;
  cout << "build at " << __TIME__ << endl;
  cout << "last changed by $Author: mschulze $" << endl << endl;

  try {
    asio::io_service io_service;

    //    tcp::endpoint endpoint(tcp::v4(), ServPort);
    EventChannelHandler localECH(io_service);//, endpoint);

    io_service.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
