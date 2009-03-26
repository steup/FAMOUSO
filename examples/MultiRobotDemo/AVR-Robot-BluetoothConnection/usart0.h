/* Includes */
#include <avr/io.h>

/* Prototypes */
void usart0_init( unsigned int baudrate );
unsigned char usart0_receive( void );
void usart0_transmit( unsigned char data );
void usart0_transmit_string(char* p);

void sendValues(char* valueDesc, int  value);
