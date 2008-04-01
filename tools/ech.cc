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

#include "util/Thread.h"
#include "devices/nic/ip/socket/TCPServerSocket.h"
#include "devices/nic/ip/socket/TCPSocket.h"

#include "famouso.h"
#include "util/endianess.h"

#define BUFSIZE 65535

typedef EventChannel<EventLayer<AbstractNetworkLayer<voidNL> > > EC;
typedef PublisherEventChannel<EC> PEC;
typedef SubscriberEventChannel<EC> SEC;

// Synchronisation der unterschiedlichen Threads beim Zugriff
// auf den ECH sollte geregelt werden

class SocketThread:public Thread {
protected:
    TCPSocket * sock;
    char echoBuffer[BUFSIZE];
    int recvMsgSize;

    SocketThread (TCPSocket * s):sock (s) {
    }

    ~SocketThread () {
        delete sock;
    }
};

class PublisherThread:public SocketThread {

    void action () {
        // Zero means end of transmission
        if ((recvMsgSize = sock->recv (echoBuffer, 8)) > 0) {
            // EventChannel mit entsprechendem Subject erstellen und ankuendigen
            PEC pec (ntohll(*(unsigned long long *) (echoBuffer)));
            pec.announce ();
            // Zugehoeriges Event erzeugen
            Event e (pec.subject ());
            cout << "Announcement for channel:\t0x" << hex << pec.
            subject ().value << endl;

            // Zero means end of transmission
            while ((recvMsgSize =
                        sock->recv (echoBuffer, 13)) > 0) {
                // ermitteln der Laenge des Events
                unsigned int len = ntohl (*(unsigned int *)&(echoBuffer[9]));
                // und den Rest aus dem Socket holen
                if ((recvMsgSize = sock->recv (echoBuffer, len)) > 0) {
                    // Event aufbauen und veroeffentlichen
                    e.setLen (len);
                    e._data = (uint8_t *) echoBuffer;
                    pec.publish (e);
                }
            }
            cout << "Unannouncement for channel:\t0x" << hex << pec.
            subject ().value << endl;
        }
    }
public:
    PublisherThread (TCPSocket * s):SocketThread (s) {
    }
};

class SubscriberThread:public SocketThread {

    void cb (EC::CallBackData & cbd) {
        uint8_t preamble[13]= {FAMOUSO::PUBLISH};
		uint64_t *sub = (uint64_t *) & preamble[1];
        uint32_t *len = (uint32_t *) & preamble[9];
		*sub = htonll(cbd._subj.value);
        *len = htonl(cbd.getLen ());
        sock->send (preamble, 13);
        sock->send (cbd._data, cbd.getLen ());
    }

    void action () {
        if ((recvMsgSize = sock->recv (echoBuffer, 8)) > 0) {	// Zero means

            // EventChannel mit entsprechendem Subject erstellen und ankuendigen
            SEC sec (ntohll(*(unsigned long long *) (echoBuffer)));
            sec.subscribe ();
            cout << "Subscription for channel:\t0x" << hex << sec.subject().value << endl;

            // 1. Callback eintragen
            sec.callback.from_function < SubscriberThread, &SubscriberThread::cb > (this);
            // 2. auf weitere Sachen auf dem Kanal warten, kann eigentlich nur die Abmeldung sein
            while ((recvMsgSize = sock->recv (echoBuffer, 1)) > 0) {
                // Zero means connection closed
            }
            cout << "Unsubscription for channel:\t0x" << hex << sec.
            subject ().value << endl;
        }
    }
public:
    SubscriberThread (TCPSocket * s):SocketThread (s) {
    }
};

class ECH {
public:
    void action () {
        try {
            TCPServerSocket servSock (echoServPort);	// Socket descriptor for server

            for (;;) {	// Run forever
                // Create separate memory for client argument
                TCPSocket *clntSock = servSock.accept ();
                {
                    char request;
                    if ((clntSock->recv (&request, 1)) >
                            0) {	// Zero means
                        switch (request) {
                        case FAMOUSO::SUBSCRIBE: {
                            // ein Interesse an einem Subject wird angekuendigt
                            //
                            /// \todo wenn eine Anwendung einen Kanal mehrfach subscribiert, werden mehrere Kanaele angelegt, aber dies ist halt so
                            /// \todo kann man aber auch abfangen und sollte man auch noch machen
                            SubscriberThread
                            *sub = new SubscriberThread (clntSock);
                            sub->start();
                            break;
                        }
                        case FAMOUSO::ANNOUNCE: {
                            // Hier muss ein neuer Kanal erstellt werden, falls er noch nicht existiert
                            PublisherThread
                            *pub = new PublisherThread (clntSock);
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
            cerr << e.what () << endl;
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
    ECH ech;
    ech.action ();
    return 0;
}
