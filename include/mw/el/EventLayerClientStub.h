#ifndef __EventLayerClientStub_h__
#define __EventLayerClientStub_h__

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "mw/api/SubscriberEventChannel.h"
#include "mw/api/PublisherEventChannel.h"

#include "famouso.h"
#include "util/endianness.h"
#include "case/Delegate.h"
#include "util/ios.h"


namespace famouso {
	namespace mw {
		namespace el {

template< class T >
class NotifyWorkerThread {

  T &sec;
public:
void action(){
    uint8_t recvBuffer[BUFSIZE];
    int recvMsgSize;
    while ((recvMsgSize = sec.snn()->receive(boost::asio::buffer(recvBuffer, 13))) > 0) {
      // ermitteln der Laenge des Events
      unsigned int len = ntohl (*(uint32_t *)&(recvBuffer[9]));
      // und den Rest aus dem Socket holen
      if ((recvMsgSize = sec.snn()->receive(boost::asio::buffer(recvBuffer, len))) > 0) {
		// Event aufbauen und veroeffentlichen
		Event e(sec.subject());
		e.length=len;
		e.data = (uint8_t *) recvBuffer;
		sec.callback(e);
	  }
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

   void do_connection_socket(famouso::mw::api::EventChannel<EventLayerClientStub> &ec) {
        ec.snn()=new boost::asio::ip::tcp::socket(famouso::util::ios::instance());
        // Establish connection with the ech
        boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string(servAddress), ServPort);
        boost::system::error_code errorc;
        ec.snn()->connect(endpoint, errorc);
        if (errorc) {
            std::cerr << "An error occurred while connecting to the ech" << std::endl;
            return;
        }
    }
 public:
    typedef boost::asio::ip::tcp::socket *SNN;

    void init(){
        famouso::util::impl::start_ios();
    }

    EventLayerClientStub() {
        init();
    }

  // announce legt hier nur einen Socket an und meldet sich
  // bei localen EventChannelHandler an
  void announce(famouso::mw::api::PublisherEventChannel<EventLayerClientStub> &ec){
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    do_connection_socket(ec);
    uint8_t transferBuffer[9] = {FAMOUSO::ANNOUNCE};
    uint64_t *sub = (uint64_t *) & transferBuffer[1];
    *sub = htonll(ec.subject().value);
    // Send the announcement to the ech
    boost::asio::write(*ec.snn(), boost::asio::buffer(transferBuffer, sizeof(transferBuffer)));
  }

  // Publish uebermittelt die Daten
  // an den localen ECH
  void publish(famouso::mw::api::PublisherEventChannel<EventLayerClientStub> &ec, const Event &e) {
    DEBUG(("%s %p\n", __PRETTY_FUNCTION__, ec.select()));
    // Hier koennte der Test erfolgen, ob die Subjects vom Event
    // und vom EventChannel gleich sind, weil nur dann
    // das Event in diesen Kanal gehoert
    //
    // Ist mit einem assert zu machen und auch ob das Subject des
    // Kanals ueberhaupt gebunden ist, aber dies ist einfach, weil sonst
    // keine Verbindung zum ech besteht und das send fehlschlaegt
    uint8_t transferBuffer[13] = {FAMOUSO::PUBLISH};
    uint64_t *sub = (uint64_t *) & transferBuffer[1];
    *sub = htonll(ec.subject().value);
    uint32_t *len = (uint32_t *) & transferBuffer[9];
    *len = htonl(e.length);
    // Send the announcement to the ech
    boost::asio::write(*ec.snn(), boost::asio::buffer(transferBuffer, sizeof(transferBuffer)));
    boost::asio::write(*ec.snn(), boost::asio::buffer(e.data, e.length));
  }

  // Verbindung  zum  ECH oeffnen und Subject subscribieren
  void subscribe(famouso::mw::api::SubscriberEventChannel<EventLayerClientStub> &ec) {
    DEBUG(("%s %p\n", __PRETTY_FUNCTION__, ec.select()));
    do_connection_socket(ec);
    // create subscribe message
    uint8_t transferBuffer[9] = {FAMOUSO::SUBSCRIBE};
    uint64_t *sub = (uint64_t *) & transferBuffer[1];
    *sub = htonll(ec.subject().value);
    // Send the announcement to the ech
    boost::asio::write(*ec.snn(), boost::asio::buffer(transferBuffer, sizeof(transferBuffer)));
    // create a thread that gets the ec and if a messages arrives at the
    // socket connection the ec is called back
    // \todo potentiell ein Speicherleck, Der Thread bzw. die Daten sollten auch wieder zerstoert werden
    //       wenn sie nicht mehr benoetigt werden
    new boost::thread(boost::bind(&NotifyWorkerThread<SEC>::action, new NotifyWorkerThread<SEC>(ec)));
    DEBUG(("Generate Thread and Connect to local ECH\n"));
  }

  // Verbindung schliessen, sollte reichen
  void unsubscribe(famouso::mw::api::SubscriberEventChannel<EventLayerClientStub> &ec) {
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    DEBUG(("close connection\n"));
  }

  // Verbindung schliessen sollte reichen
  void unannounce(famouso::mw::api::PublisherEventChannel<EventLayerClientStub> &ec) {
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    DEBUG(("close connection\n"));
  }
};
		} // namespace el
	} // namespace mw
} //namespace famouso

#endif
