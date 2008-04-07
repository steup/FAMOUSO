#include "mw/api/EventChannel.h"

#include "util/TCPSocketThread.h"

#include "famouso.h"
#include "util/endianess.h"
#include "case/Delegate.h"

template< class T >
class NotifyWorkerThread : public TCPSocketThread {

  T &sec;
  void action(){
    uint8_t recvBuffer[BUFSIZE];
    int recvMsgSize;
    while ((recvMsgSize = sec.snn()->recv (recvBuffer, 13)) > 0) {
      // ermitteln der Laenge des Events
      unsigned int len = ntohl (*(unsigned int *)&(recvBuffer[9]));
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

  // announce legt hier nur einen Thread an und meldet sich
  // bei localen EventChannelHandler an
  void announce(EventChannel<EventLayerClientStub> &ec){
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
  void publish(EventChannel<EventLayerClientStub> &ec, const Event &e) {
    DEBUG(("%s %p\n", __PRETTY_FUNCTION__, ec.select()));
    // Hier koennte der Test erfolgen, ob die Subjects vom Event
    // und vom EventChannel gleich sind, weil nur dann
    // das Event in diesen Kanal gehoert
    //
    // Ist mit einem assert zu machen und auch ob das Subject des
    // Kanals ueberhaupt gebunden ist
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
  void subscribe(EventChannel<EventLayerClientStub> &ec) {
    DEBUG(("%s %p\n", __PRETTY_FUNCTION__, ec.select()));
    ec.snn() = new TCPSocket(); 

    // Establish connection with the ech
    ec.snn()->connect(servAddress, ServPort);

    // create announcement message
    uint8_t transferBuffer[9] = {FAMOUSO::SUBSCRIBE};
    uint64_t *sub = (uint64_t *) & transferBuffer[1];
    *sub = htonll(ec.subject().value);

    // Send the announcement to the ech
    ec.snn()->send(transferBuffer, 9);
    
    // create a thread that gets the ec and if a messages arrives at the
    // socket connection the ec is called back
    /// \todo potentiell ein Speicherleck, Der Thread bzw. die Daten sollten auch wieder zerstoert werden
    ///       wenn sie nicht mehr benoetigt werden
    NotifyWorkerThread<EventChannel<EventLayerClientStub> > *nwt = new NotifyWorkerThread<EventChannel<EventLayerClientStub> >(ec);
    nwt->start();
    DEBUG(("Generate Thread and Connect to local ECH\n"));
  }

  // Verbindung schliessen, sollte reichen
  void unsubscribe(EventChannel<EventLayerClientStub> &ec) {
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    DEBUG(("close connection\n"));
  }

  // Verbindung schliessen sollte reichen
  void unannounce(EventChannel<EventLayerClientStub> &ec) {
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    DEBUG(("close connection\n"));
  }
};


// static int cd = -1;  /* connection file descriptor */
// static int is_connected = 0;

// /* Report an error to stderr, but only once. */
// static void error(char *txt)
// {
// 	static int already_reported_an_error = 0;

// 	if(!already_reported_an_error) {
// 		already_reported_an_error = 1;
// 		perror(txt);
// 	}
// }


// /*
//  * The following internal functions deal with the connection to the event channel handler.
//  */
// static int do_create_socket(void)
// {
// 	if(cd>0)
// 		return 0;

// 	if((cd = socket(AF_INET, SOCK_STREAM, 0))<0) {
// 		error("[pubsub client] can't create a unix-domain socket");
// 		return 0;
// 	}

// 	return 1;
// }

// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>

// static int do_connect(void)
// {

// #define SERV_TCP_PORT   5005
// 	struct sockaddr_in serv_addr;
// 	if(is_connected)
// 		return 0;

// 	if(cd < 0)
// 		if(!do_create_socket())
// 			return 0;
// 	bzero((char *) &serv_addr, sizeof(serv_addr));
// 	serv_addr.sin_family   = AF_INET;
// 	serv_addr.sin_addr.s_addr  = inet_addr("127.0.0.1");
// //	serv_addr.sin_addr.s_addr  = inet_addr("192.168.0.122");
// 	serv_addr.sin_port   = htons(SERV_TCP_PORT);


// 	if(connect(cd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))){
// //connect(cd, (struct sockaddr *)&sau, sizeof(sau))) {
// 		error("[pubsub client] unable to connect to the ECH");
// 		return 0;
// 	}
// 	fcntl(cd, F_SETFL, fcntl(cd, F_GETFL) | O_NONBLOCK | O_ASYNC);

// 	is_connected = 1;
// 	return 1;
// }


// static int reset_connection(void)
// {
// 	if(!is_connected)
// 		return 0;

// 	close(cd);
// 	is_connected = 0;
// 	cd = -1;

// 	return 0;
// }


// /*
//  * Write a request of the type "opcode" and length len to the ECH.
//  */
// static int writereq(unsigned char opcode, void *data, int len)
// {
// 	int rv;

// 	if(!is_connected)
// 		if(!do_connect())
// 			return 0;

// 	rv = write(cd, data, len);
// 	if(rv!=len) {
// 		reset_connection();
// 		return 0;
// 	}

// 	return 1;
// }


// int famouso_announce(famouso_subject_t chn)
// {
// 	char d[1+sizeof(famouso_subject_t)];
// 	maken64(d+1, chn);
// 	d[0]=(unsigned char)FAMOUSO::ANNOUNCE;
// 	return writereq(FAMOUSO::ANNOUNCE, d, sizeof(d));
// }

// int famouso_subscribe(famouso_subject_t chn)
// {
// 	char d[1+sizeof(famouso_subject_t)];
// 	maken64(d+1,chn);
// 	d[0]=(unsigned char)FAMOUSO::SUBSCRIBE;
// 	return writereq(FAMOUSO::SUBSCRIBE, d, sizeof(d));
// }


// int famouso_unsubscribe(famouso_subject_t chn)
// {
// 	char d[1+sizeof(famouso_subject_t)];
// 	maken64(d+1,chn);
// 	d[0]=(unsigned char)FAMOUSO::UNSUBSCRIBE;
// 	return writereq(FAMOUSO::UNSUBSCRIBE, d, sizeof(d));
// }


// int famouso_publish(famouso_event_t event)
// {
// 	char d[1+sizeof(famouso_subject_t)+4+MAX_EVENT_DATA_LENGTH];
// 	maken64(d+1, event.subject);
// 	maken32(&d[sizeof(famouso_subject_t)+1], event.len);
// 	memcpy(&d[sizeof(famouso_subject_t)+4+1], event.data, (event.len>MAX_EVENT_DATA_LENGTH) ? MAX_EVENT_DATA_LENGTH :event.len);
// 	d[0]=(unsigned char)FAMOUSO::PUBLISH;
// 	return writereq(FAMOUSO::PUBLISH, d, sizeof(d));
// }


// int famouso_poll(famouso_event_t *event) {
// 	char data[1+sizeof(famouso_subject_t)+4+MAX_EVENT_DATA_LENGTH+4096];
// 	int rv=0, c=13, i=0;

// 	if(!is_connected)
// 		if(!do_connect())
// 			return 0;

// 	rv = read(cd, &data[i], c);

// 	/* no message */
// 	if((rv < 0) && (errno == EAGAIN))
// 		return 0;

// 	/* error */
// 	if(rv < 0) {
// 		error("connection reseted\n");
// 		reset_connection();
// 		return 0;
// 	}
// //	printf("0x%x FAMOUSO::PUBLISH: 0x%x ECH_MSG_PUBLISHED: 0x%x\n",data[0],FAMOUSO::PUBLISH, ECH_MSG_PUBLISHED);
// 	switch(data[0]) {
// //		case ECH_MSG_PUBLISHED:
// 		case FAMOUSO::PUBLISH:
// #warning "Preliminary Version. Something to do, but for the first strike it should do"
// 			// lesen der ersten 13 Zeichen, was bedeutet, wir haben
// 			// den Nachrichten Type gelesen sowie das Subject und
// 			// die Laenge
// 			c-=rv;
// 			i=rv;
// 			while (c) {
// 				rv = read(cd, &data[i], c);
// 				if (rv<0) {
// 					if (errno != EAGAIN){
// 						printf("connection reseted");
// 						abort();
// 					}
// 				} else {
// 					c-=rv;
// 					i+=rv;
// 				}
// 			}

// 			event->subject = make64n(data+1);
// 			event->len = ntohl(*(uint32_t*)&data[9]);
// //			printf("Length: %4d \tSubject: 0x%16llx\n",event->len, event->subject);
// 			c=event->len;
// 			i=0;
// 			while (c) {
// 				rv = read(cd, &data[i], c);
// 				if (rv<0) {
// 					if (errno != EAGAIN){
// 						printf("connection reseted");
// 						abort();
// 					}
// 				} else {
// 					c-=rv;
// 					i+=rv;
// 				}
// 			}

// 			if( event->len > MAX_EVENT_DATA_LENGTH )
// 				event->len = MAX_EVENT_DATA_LENGTH;
// 			for(i=0; i<event->len; i++)
// 				event->data[i] = data[i];
// //			printf("Count of Bytes Read: %d Length: %4d \tSubject: 0x%16llx\n",rv,event->len, event->subject);
// 			return 1;
// 	}

// 	error("[pubsub client] received unknown opcode from ECH");
// 	return 0;
// }
