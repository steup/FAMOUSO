#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "usart0.h"
#include "Analog.h"

#include <stdio.h>
#include "defRobby.h"

// standard velocity
#define STD_VELOCITY 350

volatile char input[5];
volatile uint8_t VirtualSensor = 255;

ISR (USART0_RX_vect)
{
	if (UDR0==65){
		for (char i= 0;i<5;i++){
		input[i]=usart0_receive();
		}
		VirtualSensor=input[0];
	}
}

static inline uint8_t get_own_ID() {
    DDRG &= ~( (1 << PG0) | (1 << PG1) );
    PORTG = (1 << PG0) | (1 << PG1);
    return (PING & 0x3);
}

enum LEDCOMBINATIONS {LLLL,
                      LLLH, LLHL, LLHH, LHLL, LHLH,
                      LHHL, LHHH, HLLL, HLLH, HLHL,
                      HLHH, HHLL, HHLH, HHHL, HHHH
                     };

enum DIRECTIONS {LEFT, RIGHT, STRAIGHT, STOP};

volatile uint8_t TicksCounterLeft=0;
volatile uint8_t TicksCounterRight=0;

ISR (INT2_vect)
{
 	TicksCounterRight++;
}

ISR (INT3_vect)
{
 	TicksCounterLeft++;
}

uint8_t getTicksCounterLeft(){
	uint8_t aux=TicksCounterLeft;
	TicksCounterLeft=0;
    return aux;
	}

uint8_t getTicksCounterRight(){
	uint8_t aux=TicksCounterRight;
	TicksCounterRight=0;
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
    PORTB &= ~ (1 << PWM1_OUT) | (1 << PWM2_OUT);
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
	EICRA|=(1<<ISC21)|(1<<ISC31);
	// enable interrupts 2 and 3
	EIMSK|=(1<<2)|(1<<3);
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
                PORTE &= ~ (1 << MOTOR1B_DIR);
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


volatile uint8_t Human = 0;
volatile uint8_t Crash = 0;

// -> Distance, Crashed_, HumanDet
// <- Velocity

int RealSensorFront = 0;
int RealSensorRight = 0;


int main() {

    init();
    // USART-Initalisierung
    usart0_init(51); // 19200 8N1 bei 16 MHz
    usart0_transmit_string("\n\rInitialisation started\n\r");

    // Interrupts erlauben
    sei();    // global enable interrupts

  	uint8_t leftTicks, rightTicks=0;
	uint8_t data[4];

	char output[50]; 

    Crash=0;
    ledOn(0);
    while (1) {
	    Analog(1);
        RealSensorFront = adc_get_value();
	    Analog(3);
        RealSensorRight = adc_get_value();
		leftTicks=getTicksCounterLeft();
		rightTicks=getTicksCounterRight();

		Human=input[2];
		Crash=input[1];


        if ((Human != 0) || (Crash==1)){
            drive(STOP);
			data[1]=0;
			data[2]=0;
        } else {
 			if ( (RealSensorFront > 200) || (RealSensorRight > 220) || (VirtualSensor < 60) ) {
                drive(LEFT);
                VirtualSensor = 255;
				if (leftTicks==0)
					data[1]=0;
				else{
		        	data[1]=256-leftTicks;
				}
		        data[2]=rightTicks;
            } else {
                drive(STRAIGHT);
		        data[1]=leftTicks;
		        data[2]=rightTicks;
            }
        }
//		sprintf(output,"B %c %c %c B\n\r",data[1],data[2],RealSensorFront);
		sprintf(output,"B %i %i %i\n\r",data[1],data[2],RealSensorFront);
		usart0_transmit_string(output);

        wait_ms(20);
    }
}
