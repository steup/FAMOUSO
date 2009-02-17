#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "usart0.h"
#include "Analog.h"

#include <stdio.h>
#include "defRobby.h"

#include "mw/nl/CANNL.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/common/Event.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"

#include "devices/nic/can/at90can/avrCANARY.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"
#include "mw/common/UID.h"
#include "famouso.h"

namespace famouso {
    namespace Robot {
        struct ID {
            enum { value = 51 };
        };
    }
    namespace CAN {
        class config {

                typedef device::nic::CAN::avrCANARY can;
                typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
                typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
                typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> nl;
                typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
                typedef famouso::mw::el::EventLayer< anl > el;
            public:
                typedef famouso::mw::api::EventChannel< el > EC;
                typedef famouso::mw::api::PublisherEventChannel<el> PEC;
                typedef famouso::mw::api::SubscriberEventChannel<el> SEC;
        };
    }

    typedef CAN::config config;
}

// standard velocity
#define STD_VELOCITY 350

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

volatile uint16_t TicksCounterLeft=0;
volatile uint16_t TicksCounterRight=0;

ISR (INT2_vect)
{
 	TicksCounterRight++;
}

ISR (INT3_vect)
{
 	TicksCounterLeft++;
}

int16_t getTicksCounterLeft(){
	int16_t aux=TicksCounterLeft;
	TicksCounterLeft=0;
    return aux;
	}

int16_t getTicksCounterRight(){
	int16_t aux=TicksCounterRight;
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
    // 0    0 Normal port operation, OCnA/OCnB/OCnC disconnected
    // 0    1
    // 1    0 Clear OCnA/OCnB/OCnC on compare match when up-counting.
    //  1    1 Set OCnA/OCnB/OCnC on compare match when up-counting.
    TCCR1A |= ((1 << COM1A1) | (1 << COM1B1));
    // enable the timer with prescaler of 1024
    //  CS12 CS11 CS10
    // 0  0   0 Stop
    // 0  0   1 CK
    // 0  1   0 CK / 8
    // 0  1   1 CK / 64
    // 1  0   0 CK / 256
    // 1  0   1 CK / 1024
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

volatile uint8_t VirtualSensor = 255;
volatile uint8_t Human = 0;

uint16_t RealSensor = 0;
char *HumanDetectionSubject = "HumanDet";
char *distance = "Distance";
char *velocity = "Velocity";

void VSensor_CB(famouso::mw::api::SECCallBackData& e) {
        if ((e.data[0] == famouso::Robot::ID::value)
//            && (e.data[2] == 'v')
            )
        VirtualSensor=e.data[1];
}

void Human_CB(famouso::mw::api::SECCallBackData& e) {
    Human=e.data[1];
}

int main() {

    init();
    // USART-Initalisierung
    usart0_init(51); // 19200 8N1 bei 16 MHz
    usart0_transmit_string("\nRobot starting\n");

    // Interrupts erlauben
    sei();    // global enable interrupts

    usart0_transmit_string("FAMOUSO -- Initialization started.\n");
    famouso::init<famouso::config::EC>();

    usart0_transmit_string("FAMOUSO -- Initialization successful.\n");

    famouso::config::SEC sec(distance);
    sec.subscribe();
    sec.callback.bind<&VSensor_CB>();

    famouso::config::SEC hec(HumanDetectionSubject);
    hec.subscribe();
    hec.callback.bind<&Human_CB>();

    famouso::config::PEC pec(velocity);
    pec.announce();
    famouso::mw::Event e(pec.subject());
    uint8_t data[3]={famouso::Robot::ID::value,0,0};
    e.length = 3;
    e.data = data;

	int16_t leftTicks, rightTicks=0;

    Analog(1);
    ledOn(0);
    while (1) {
        RealSensor = adc_get_value();
		leftTicks=getTicksCounterLeft();
		rightTicks=getTicksCounterRight();
        if (Human != 0) {
            drive(STOP);
        } else {
            if ( (RealSensor > 200) || (VirtualSensor < 60) ) {
                drive(LEFT);
                VirtualSensor = 255;
            } else {
                drive(STRAIGHT);
            }
        }
		data[1]=leftTicks;
		data[2]=rightTicks;
        pec.publish(e);
        for ( uint32_t o = 100000;o > 0;--o)
            asm volatile ("nop \n\t");
    }
}
