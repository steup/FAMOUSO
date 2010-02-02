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

/**
* \file defRobby
* \author Sebastian Zug
* \date 09.10.2007
* \brief Definition of the connections between At90Can128 and robby
*
* \version 0.1
*/

#ifndef __def_h__
#define __def_h__

#ifndef F_CPU
#define F_CPU 16000000UL  // 16 MHz
#endif
#include <util/delay.h>

/*! Timer functions */
inline void wait_ms(int delay) {
    for (int i = 0;i < delay / 10;i++) _delay_ms(10);
}
inline void wait_s(int delay) {
    for (int i = 0;i < delay;i++) wait_ms(1000);
}

/*! Makro for button pressed */
#define BUTTONpressed(x) ((PINA & (16<<x))==0)

/*! Makros for led control*/
#define ledOn(x)  (PORTA |=_BV(x))
#define ledOff(x) (PORTA &=~_BV(x))

/*! Definitions of port IDs*/
//--------------------------------------------------- Port A ---------------------------------------------
#define LED_0  0
#define LED_1  1
#define LED_2       2
#define LED_3       3
#define BUTTON_0 4
#define BUTTON_1 5
#define BUTTON_2 6
#define BUTTON_3 7
//--------------------------------------------------- Port B ---------------------------------------------
//#define   0
//#define   1
//#define   2
//#define   3
//#define   4
#define PWM1_OUT 5
#define PWM2_OUT 6
#define PWM3_OUT 7  // On Board LED
//--------------------------------------------------- Port C ---------------------------------------------
#define VSS_0  0
#define VSS_1  1
#define VSS_2  2
#define VSS_3  3
#define VSS_4  4
#define VSS_5  5
#define VSS_6  6
#define VSS_7  7
//--------------------------------------------------- Port D ---------------------------------------------
//#define   0
//#define   1
#define INT_2  2
#define INT_3  3
//#define   4
//#define   5
//#define   6
//#define   7
//--------------------------------------------------- Port E ---------------------------------------------
//#define   0
//#define   1
#define MOTOR1A_DIR 2
#define MOTOR1B_DIR 3
//#define   4
//#define   5
//#define   6
//#define   7
//--------------------------------------------------- Port F ---------------------------------------------
#define ANALOG_0 0
#define ANALOG_1 1
#define ANALOG_2 2
#define ANALOG_3 3
#define ANALOG_4 4
#define ANALOG_5 5
//#define   6
//#define   7
//--------------------------------------------------- Port G ---------------------------------------------
#define MOTOR2A_DIR 0
#define MOTOR2B_DIR 1
//#define   2
//#define   3
//#define   4
//#define   5
//#define   6
//#define   7

#endif
