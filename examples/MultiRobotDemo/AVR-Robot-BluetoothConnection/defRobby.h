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
static inline void wait_ms(int delay) {
    for (int i = 0;i < delay / 10;i++) _delay_ms(10);
}
static inline void wait_s(int delay) {
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
