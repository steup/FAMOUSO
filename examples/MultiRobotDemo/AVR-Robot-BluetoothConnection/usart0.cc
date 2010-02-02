/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/

/* Includes */
#include <avr/io.h>
#include <stdlib.h>
#include "usart0.h"


/* Initialize UART */
void usart0_init(unsigned int baudrate) {
    /* Set the baud rate */
    UBRR0H = (unsigned char)(baudrate >> 8);
    UBRR0L = (unsigned char) baudrate;

    /* Enable UART receiver and transmitter */
    UCSR0B = ((1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0));

    /* Set frame format: 8N1 */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}


/* Read and write functions */
unsigned char usart0_receive(void) {
    /* Wait for incomming data */
    while (!(UCSR0A & (1 << RXC0)));
    /* Return the data */
    return UDR0;
}

void usart0_transmit(unsigned char data) {
    /* Wait for empty transmit buffer */
    while (!(UCSR0A & (1 << UDRE0)));
    /* Start transmittion */
    UDR0 = data;
}

void usart0_transmit_string(char* p) {
    while (*p)
        usart0_transmit(*p++);
}

void sendValues(char* valueDesc, int  value) {
    char text[20] = {0};
    char *t;
    t = itoa(value, text, 10);
    usart0_transmit_string(valueDesc);
    usart0_transmit_string(t);
    usart0_transmit('\n');
}
