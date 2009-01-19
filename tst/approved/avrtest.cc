#include <stddef.h>
#include <stdint.h>

//#include "mw/nl/CANNL.h"
#include "mw/nl/voidNL.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/common/Event.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

//#include "devices/nic/can/peak/PeakCAN.h"
//#include "mw/nl/can/canETAGS.h"
//#include "mw/nl/can/canID.h"
//#include "mw/nl/can/etagBP/Client.h"
//#include "mw/nl/can/ccp/Client.h"
#include "mw/common/UID.h"
#include "famouso.h"

typedef famouso::mw::nl::voidNL nl;
typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
typedef famouso::mw::el::EventLayer< anl > el;
typedef famouso::mw::api::EventChannel< el > EC;
typedef famouso::mw::api::PublisherEventChannel<el> PEC;
typedef famouso::mw::api::SubscriberEventChannel<el> SEC;


#ifndef AVR
#define DEBUG(x) printf x
void done() {}
void init() {}
#else 
inline void init() {}

#include <avr/io.h>

//inline void usart1_transmit( unsigned char data )
//{
//        /* Wait for empty transmit buffer */
//        while ( !(UCSR1A & (1<<UDRE0)) );
//        /* Start transmittion */
//        UDR1 = data;
//}
//
///* Initialize UART */
//inline void init() {
//        /* Set the baud rate */
//        UBRR1H = (unsigned char) (51>>8);
//        UBRR1L = (unsigned char) 51;
//
//        /* Enable UART receiver and transmitter */
//        UCSR1B = ( ( 1 << RXEN1 ) | ( 1 << TXEN1 ) );
//
//        /* Set frame format: 8N1 */
//        UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);
//}
//
//inline void usart1_transmit_string(char* p){
//  while (*p)
//    usart1_transmit(*p++);
//}
//
#define DEBUG(X)
//usart1_transmit_string X
inline void done() { while(1);}
#endif

void cb(famouso::mw::api::SECCallBackData& cbd) {
  DEBUG(("\r\nMichaels CallBack\r\n\r\n"));
}

int  main() {
    init();
        // greet the world
    DEBUG(("Hello World\r\n"));
        // init the can interface
	famouso::init<EC>();
	SEC sec(0xf1);
	sec.subscribe();
	sec.callback.bind<&cb>();

	PEC pec(0xf1);
	pec.announce();

	famouso::mw::Event e(pec.subject());
	uint8_t data[3]={1,50,'v'};
	e.length = 3;
	e.data = data;

	pec.publish(e);
        DEBUG(("publishing done\r\n"));

	done();
}
