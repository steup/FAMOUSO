/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "usart0.h"
#include "Analog.h"

#include <stdio.h>
#include "defRobby.h"

// standard velocity
#define STD_VELOCITY 600
#define DISTANCE_LIMIT 160

volatile char input[5];
volatile uint8_t VirtualSensor = 255;
volatile unsigned char Human = 0;
volatile unsigned char Crash = 0;
bool fahr_nich = false;
volatile char SteeringLeft = 0;
volatile char SteeringRight = 0;

ISR(USART0_RX_vect) {

    if (UDR0 == 65) {
        VirtualSensor = usart0_receive();
        Human = usart0_receive();
        Crash = usart0_receive();
        SteeringLeft = usart0_receive();
        SteeringRight = usart0_receive();
    }
}

static inline uint8_t get_own_ID() {
    DDRG &= ~((1 << PG0) | (1 << PG1));
    PORTG = (1 << PG0) | (1 << PG1);
    return (PING & 0x3);
}

enum LEDCOMBINATIONS {LLLL,
                      LLLH, LLHL, LLHH, LHLL, LHLH,
                      LHHL, LHHH, HLLL, HLLH, HLHL,
                      HLHH, HHLL, HHLH, HHHL, HHHH
                     };

enum DIRECTIONS {LEFT, RIGHT, STRAIGHT, STOP};

volatile uint8_t TicksCounterLeft = 0;
volatile uint8_t TicksCounterRight = 0;

ISR(INT2_vect) {
    TicksCounterRight++;
}

ISR(INT3_vect) {
    TicksCounterLeft++;
}

uint8_t getTicksCounterLeft() {
    uint8_t aux = TicksCounterLeft;
    TicksCounterLeft = 0;
    return aux;
}

uint8_t getTicksCounterRight() {
    uint8_t aux = TicksCounterRight;
    TicksCounterRight = 0;
    return aux;
}


void init() {
    //--------------------------------------------------- Port A ---------------------------------------------
    // set switches BUTTON_X as inputs
    DDRA &= ~((1 << BUTTON_0) | (1 << BUTTON_1) | (1 << BUTTON_2) | (1 << BUTTON_3));
    // set leds LED_X as output
    DDRA |= ((1 << LED_0) | (1 << LED_1) | (1 << LED_2) | (1 << LED_3));
    // disable leds
    PORTA &= ~((1 << LED_0) | (1 << LED_1) | (1 << LED_2) | (1 << LED_3));
    //--------------------------------------------------- Port B ---------------------------------------------
    // set PWMX_OUT pins as outputs
    DDRB |= (1 << PWM1_OUT) | (1 << PWM2_OUT) | (1 << PWM3_OUT);
    // switch off motor driver (L293E)
    PORTB &= ~(1 << PWM1_OUT) | (1 << PWM2_OUT);
    //--------------------------------------------------- Port C ---------------------------------------------
    // set pins for supply voltage as outputs
    DDRC |= ((1 << VSS_0) | (1 << VSS_1) | (1 << VSS_2) | (1 << VSS_3) | (1 << VSS_4) | (1 << VSS_5) | (1 << VSS_6) | (1 << VSS_7));
    // power on for external sensors / devices
    // we use npn transistors so "0" means power on and "1" power off
    PORTC &= ~((1 << VSS_0) | (1 << VSS_1) | (1 << VSS_2) | (1 << VSS_3) | (1 << VSS_4) | (1 << VSS_5) | (1 << VSS_6) | (1 << VSS_7));
    //--------------------------------------------------- Port D ---------------------------------------------
    // INT_2 and INT_3 as inputs
    DDRD &= ~((1 << INT_2) | (1 << INT_3));
    //--------------------------------------------------- Port E ---------------------------------------------
    // direction pins for first motor as output
    DDRE |= (1 << MOTOR1A_DIR) | (1 << MOTOR1B_DIR);
    //--------------------------------------------------- Port F ---------------------------------------------
    // all ANALOG_X as inputs
    DDRF &= ~((1 << ANALOG_0) | (1 << ANALOG_1) | (1 << ANALOG_2) | (1 << ANALOG_3) | (1 << ANALOG_4) | (1 << ANALOG_5));
    //--------------------------------------------------- Port G ---------------------------------------------
    // direction pins for second motor as output
    DDRG |= (1 << MOTOR2A_DIR) | (1 << MOTOR2B_DIR);
    //--------------------------------------------------- PWM Generation -------------------------------------
    // PWM phase correct mode (10bit)
    TCCR1A |= (1 << WGM10) | (1 << WGM11);
    //  COM1A1 COM1A0
    //  0    0 Normal port operation, OCnA/OCnB/OCnC disconnected
    //  0    1
    //  1    0 Clear OCnA/OCnB/OCnC on compare match when up-counting.
    //  1    1 Set OCnA/OCnB/OCnC on compare match when up-counting.
    TCCR1A |= ((1 << COM1A1) | (1 << COM1B1));
    // enable the timer with prescaler of 1024
    // CS12 CS11 CS10
    // 0    0    0    Stop
    // 0    0    1    CK
    // 0    1    0    CK / 8
    // 0    1    1    CK / 64
    // 1    0    0    CK / 256
    // 1    0    1    CK / 1024
    TCCR1B |= ((1 << CS10) | (1 << CS11));
    //-------------------------------------------- Init external Interrups ---------------------------------------
    // trigger interrupt with rising edge
    EICRA |= (1 << ISC21) | (1 << ISC31);
    // enable interrupts 2 and 3
    EIMSK |= (1 << 2) | (1 << 3);
}

void LedsOnByValue(LEDCOMBINATIONS value) {
    char aux = PORTA;
    aux &= ~((1 << LED_0) | (1 << LED_1) | (1 << LED_2) | (1 << LED_3));
    PORTA = (aux | value);
}

void drive(DIRECTIONS dir) {
    OCR1AH = (STD_VELOCITY >> 8);
    OCR1AL = (STD_VELOCITY &  0xff);
    OCR1BH = (STD_VELOCITY >> 8);
    OCR1BL = (STD_VELOCITY &  0xff);
    switch (dir) {
        case STOP : {
                PORTG &= ~(1 << MOTOR2A_DIR);
                PORTG &= ~(1 << MOTOR2B_DIR);
                PORTE &= ~(1 << MOTOR1A_DIR);
                PORTE &= ~(1 << MOTOR1B_DIR);
                break;
            }
        case STRAIGHT : {
                PORTG &= ~(1 << MOTOR2A_DIR);
                PORTG |= (1 << MOTOR2B_DIR);
                PORTE |= (1 << MOTOR1A_DIR);
                PORTE &= ~(1 << MOTOR1B_DIR);
                break;
            }
        case RIGHT : {
                PORTG |= (1 << MOTOR2A_DIR);
                PORTG &= ~(1 << MOTOR2B_DIR);
                PORTE |= (1 << MOTOR1A_DIR);
                PORTE &= ~(1 << MOTOR1B_DIR);
                break;
            }
        case LEFT : {
                PORTG &= ~(1 << MOTOR2A_DIR);
                PORTG |= (1 << MOTOR2B_DIR);
                PORTE &= ~(1 << MOTOR1A_DIR);
                PORTE |= (1 << MOTOR1B_DIR);
                break;
            }
    }
}



// -> Distance, Crashed_, HumanDet
// <- Velocity

int RealSensorFront = 0;
int RealSensorRight = 0;
int RealSensorLeft = 0;


int main() {

    init();
    // USART-Initalisierung
    usart0_init(51); // 19200 8N1 bei 16 MHz
    usart0_transmit_string("\n\rInitialisation started\n\r");

    // Interrupts erlauben
    sei();    // global enable interrupts

    uint8_t leftTicks, rightTicks = 0;
    uint8_t data[4];
    int8_t delay = 0;

    char output[50];

    ledOn(2);

    while (!BUTTONpressed(2));

    while (1) {
        Analog(0);
        RealSensorFront = adc_get_value();
        if (RealSensorFront > DISTANCE_LIMIT)
            ledOn(1);
        else
            ledOff(1);
        Analog(3);
        RealSensorRight = adc_get_value();
        if (RealSensorRight > DISTANCE_LIMIT)
            ledOn(2);
        else
            ledOff(2);
        Analog(4);
        RealSensorLeft = adc_get_value();
        if (RealSensorLeft > DISTANCE_LIMIT)
            ledOn(3);
        else
            ledOff(3);

        leftTicks = getTicksCounterLeft();
        rightTicks = getTicksCounterRight();

        if (Human == 2) fahr_nich = false;

        if ((Human == 1) || (Crash == 1)) {
            drive(STOP);
            data[1] = 0;
            data[2] = 0;
            fahr_nich = true;

		if (  (RealSensorFront > DISTANCE_LIMIT) || (RealSensorRight > DISTANCE_LIMIT) || (VirtualSensor < 75)) {
            if (!fahr_nich) drive(LEFT);
            delay = 0;
            if (leftTicks == 0)
                data[1] = 0;
            else {
                data[1] = 256 - leftTicks;
            }
            data[2] = rightTicks;
        } else {
            if (!fahr_nich) drive(STRAIGHT);
            data[1] = leftTicks;
            data[2] = rightTicks;
        }

//  sprintf(output,"B %c %c %c B\n\r",data[1],data[2],RealSensorFront);
        sprintf(output, "B %i %i %i\n\r", data[1], data[2], 255);
        usart0_transmit_string(output);

        wait_ms(50);
    }
}
