#ifndef __EventLayerClientStub_h__
#define __EventLayerClientStub_h__


#include "mw/api/SubscriberEventChannel.h"
#include "mw/api/PublisherEventChannel.h"

#include "util/TCPSocketThread.h"

#include "famouso.h"
#include "util/endianess.h"
#include "case/Delegate.h"

namespace famouso {
	namespace mw {
		namespace el {

template< class T >
class NotifyWorkerThread : public TCPSocketThread {

  T &sec;
  void action(){
    uint8_t recvBuffer[BUFSIZE];
    int recvMsgSize;
    while ((recvMsgSize = sec.snn()->recv (recvBuffer, 13)) > 0) {
      // ermitteln der Laenge des Events
      unsigned int len = ntohl (*(uint32_t *)&(recvBuffer[9]));
      // und den Rest aus dem Socket holen
      if ((recvMsgSize = sec.snn()->recv (recvBuffer, len)) > 0) {
		// Event aufbauen und veroeffentlichen
		Event e(sec.subject());
		e.length=len;
		e.data = (uint8_t *) recvBuffer;
		sec.callback(e);
	  }
	}
  }
 public:
 NotifyWorkerThread(T &ec) : TCPSocketThread (ec.snn()), sec(ec) {
  }
};

class EventLayerClientStub {
 public:
  typedef TCPSocket* SNN;

  // announce legt hier nur einen Socket an und meldet sich
  // bei localen EventChannelHandler an
  void announce(famouso::mw::api::PublisherEventChannel<EventLayerClientStub> &ec){
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    ec.snn() = new TCPSocket(); 
    // Establish connection with the ech
    ec.snn()->connect(servAddress, ServPort);
    uint8_t transferBuffer[9] = {FAMOUSO::ANNOUNCE};
    uint64_t *sub = (uint64_t *) & transferBuffer[1];
    *sub = htonll(ec.subject().value);
    // Send the announcement to the ech
    ec.snn()->send(transferBuffer, 9);
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
    ec.snn()->send(transferBuffer, 13);

    ec.snn()->send(e.data,e.length);
  }

  // Verbindung  zum  ECH oeffnen und Subject subscrebieren
  void subscribe(famouso::mw::api::SubscriberEventChannel<EventLayerClientStub> &ec) {
    DEBUG(("%s %p\n", __PRETTY_FUNCTION__, ec.select()));
    ec.snn() = new TCPSocket(); 

    // Establish connection with the ech
    ec.snn()->connect(servAddress, ServPort);

    // create subscribe message
    uint8_t transferBuffer[9] = {FAMOUSO::SUBSCRIBE};
    uint64_t *sub = (uint64_t *) & transferBuffer[1];
    *sub = htonll(ec.subject().value);

    // Send the subscribe to the ech
    ec.snn()->send(transferBuffer, 9);

    // create a thread that gets the ec and if a messages arrives at the
    // socket connection the ec is called back
    /// \todo potentiell ein Speicherleck, Der Thread bzw. die Daten sollten auch wieder zerstoert werden
    ///       wenn sie nicht mehr benoetigt werden
	typedef famouso::mw::api::SubscriberEventChannel<EventLayerClientStub> SEC;
    NotifyWorkerThread<SEC> *nwt = new NotifyWorkerThread<SEC>(ec);
    nwt->start();
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
