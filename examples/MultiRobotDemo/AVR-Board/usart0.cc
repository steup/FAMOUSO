/* Includes */
#include <avr/io.h>
#include <stdlib.h>
#include "usart0.h"

/* Initialize UART */
void usart0_init( unsigned int baudrate ) {
	/* Set the baud rate */
	UBRR0H = (unsigned char) (baudrate>>8);
	UBRR0L = (unsigned char) baudrate;

	/* Enable UART receiver and transmitter */
	UCSR0B = ( ( 1 << RXEN0 ) | ( 1 << TXEN0 ) );

	/* Set frame format: 8N1 */
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}


/* Read and write functions */
unsigned char usart0_receive( void ) {
	/* Wait for incomming data */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* Return the data */
	return UDR0;
}

void usart0_transmit( unsigned char data ) {
	/* Wait for empty transmit buffer */
	while ( !(UCSR0A & (1<<UDRE0)) );
	/* Start transmittion */
	UDR0 = data;
}

void usart0_transmit_string(char* p){
	while (*p)
		usart0_transmit(*p++);
}

void sendValues(char* valueDesc, unsigned int  value){
    char text[20]={0};
    char *t;
    t=utoa(value,text,10);
    usart0_transmit_string(valueDesc);
    usart0_transmit_string(t);
    usart0_transmit('\n');

}
void sendValues(char* valueDesc, int  value){
    char text[20]={0};
    char *t;
    t=itoa(value,text,10);
    usart0_transmit_string(valueDesc);
    usart0_transmit_string(t);
    usart0_transmit('\n');
}
