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

typedef famouso::mw::nl::voidNL	nl;
typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
typedef famouso::mw::el::EventLayer< anl > el;
typedef famouso::mw::api::EventChannel< el > EC;
typedef famouso::mw::api::PublisherEventChannel<EC> PEC;
typedef famouso::mw::api::SubscriberEventChannel<EC> SEC;

class PublisherThread : public TCPSocketThread {
  void action () {
    char echoBuffer[BUFSIZE];
    int recvMsgSize;

    // Zero means end of transmission
    if ((recvMsgSize = sock->recv (echoBuffer, 8)) > 0) {
      // EventChannel mit entsprechendem Subject erstellen und ankuendigen
      PEC pec (ntohll(*(unsigned long long *) (echoBuffer)));
      pec.announce ();
      // Zugehoeriges Event erzeugen
      famouso::mw::Event e (pec.subject ());
      cout << "Announcement for channel:\t0x" << hex << pec.subject().value << endl;

      // Zero means end of transmission
      while ((recvMsgSize = sock->recv (echoBuffer, 13)) > 0) {
	// ermitteln der Laenge des Events
	unsigned int len = ntohl (*(unsigned int *)&(echoBuffer[9]));
	// und den Rest aus dem Socket holen
	if ((recvMsgSize = sock->recv (echoBuffer, len)) > 0) {
	  // Event aufbauen und veroeffentlichen
	  e.length=len;
	  e.data = (uint8_t *) echoBuffer;
	  pec.publish (e);
	}
      }
      cout << "Unannouncement for channel:\t0x" << hex << pec.subject ().value << endl;
    }
  }
public:
  PublisherThread (TCPSocket * s):TCPSocketThread (s) {
  }

  ~PublisherThread () {
//    cout << "~PublisherThread" <<endl;
  }

};

class SubscriberThread:public TCPSocketThread {

  // wenn die Verbindung wegstirbt, sollte eine Rueckkopplung in die Action erfolgen
  // damit die Verbindung sauber geschlossen werden kann, ohne das der ECH mit
  // ins Nirvana gerissen wird
  void cb (famouso::mw::aux::CallBackData & cbd) {
    uint8_t preamble[13]= {FAMOUSO::PUBLISH};
    uint64_t *sub = (uint64_t *) & preamble[1];
    uint32_t *len = (uint32_t *) & preamble[9];
    *sub = htonll(cbd.subject.value);
    *len = htonl(cbd.length);
    sock->send (preamble, 13);
    sock->send (cbd.data, cbd.length );
  }

  void action () {
    char echoBuffer[BUFSIZE];
    int recvMsgSize;
    if ((recvMsgSize = sock->recv (echoBuffer, 8)) > 0) {	// Zero means

      // EventChannel mit entsprechendem Subject erstellen
      SEC sec (ntohll(*(uint64_t *) (echoBuffer)));
      // Callback eintragen
      // und ankuendigen
      sec.subscribe ();
      cout << "Subscription for channel:\t0x" << hex << sec.subject().value << endl;
      sec.callback.from_function < SubscriberThread, &SubscriberThread::cb > (this);
      
      // auf weitere Sachen auf dem Kanal warten, kann eigentlich nur die Abmeldung sein
      while ((recvMsgSize = sock->recv (echoBuffer, 1)) > 0) {
	// Zero means connection closed
      }
      cout << "Unsubscription for channel:\t0x" << hex << sec.subject ().value << endl;
    }
  }
public:
  SubscriberThread (TCPSocket * s):TCPSocketThread (s) {
  }

  ~SubscriberThread () {
//    cout << "~SubscriberThread" <<endl;
  }

};

class ECH {
public:
  void action () {
    try {
      TCPServerSocket servSock (ServPort);	// Socket descriptor for server

      for (;;) {	// Run forever
	// Create separate memory for client argument
	TCPSocket *clntSock = servSock.accept ();
	{
	  char request;
	  if ((clntSock->recv (&request, 1)) > 0) {	// Zero means
	    switch (request) {
	    case FAMOUSO::SUBSCRIBE: {
	      // ein Interesse an einem Subject wird angekuendigt
	      //
	      /// \todo wenn eine Anwendung einen Kanal mehrfach subscribiert, werden mehrere Kanaele angelegt, aber dies ist halt so
	      /// \todo kann man aber auch abfangen und sollte man auch noch machen
	      SubscriberThread *sub = new SubscriberThread (clntSock);
	      sub->start();
	      break;
	    }
	    case FAMOUSO::ANNOUNCE: {
	      // Hier muss ein neuer Kanal erstellt werden, falls er noch nicht existiert
	      PublisherThread *pub = new PublisherThread (clntSock);
	      pub->start ();
	      break;
	    }
	    default:
	      delete clntSock;
	      cout << "wrong format connection closed" << endl;
	    }
	  }
	}
      }
    } catch (SocketException & e) {
      cerr << e.what () << "Michael "<< endl;
      exit (1);
    }
  }			// NOT REACHED
};


int main (int argc, char **argv) {
  cout << "Project: FAMOUSO" << endl;
  cout << "local Event Channel Handler" << endl << endl;
  cout << "Author: Michael Schulze" << endl;
  cout << "Revision: $Rev$" << endl;
  cout << "$Date$" <<endl; 
  cout << "build at " << __TIME__ << endl;
  cout << "last changed by $Author$" << endl << endl;

//  SEC sec(0xf200000000000000ll);
//  sec.subscribe();

  ECH ech;
  ech.action ();
  return 0;
}
