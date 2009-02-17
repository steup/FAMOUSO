/*
 * RobotDemo.cc
 *
 *
 *  Michael Schulze, 2008
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include "famouso_bindings.h"
#include "util/endianness.h"

#define RobotID 4

int done;
char *HumanDetectionSubject="HumanDet";
char *Distance="Distance";
char *Velocity="Velocity";

volatile uint8_t DistanceSensorValue=200;

void SensorValueCB(famouso::mw::api::SECCallBackData& cbd) {
    if ((cbd.data[2] == RobotID)){
        DistanceSensorValue=cbd.data[0];
    }
}

volatile uint8_t Human=0;
void HumanDetectedCB(famouso::mw::api::SECCallBackData& cbd) {
    Human=cbd.data[0];
};

void siginthandler(int egal) {
    done = 1;
}

int main(int argc, char **argv) {
    done = 0;
    signal(SIGINT,siginthandler);

    printf("FAMOUSO -- Initialization started.\n");
    printf("FAMOUSO -- Initialization successful.\n");

    famouso::config::SEC sec(Distance);
    sec.subscribe();
    sec.callback.bind<SensorValueCB>();

    famouso::config::SEC H(HumanDetectionSubject);
    H.subscribe();
    H.callback.bind<HumanDetectedCB>();

    famouso::config::PEC pec(Velocity);
    famouso::mw::Event e(pec.subject());
	uint8_t data[3]={120,120,RobotID};
	e.length = 3;
	e.data = data;
    pec.announce();

    while(!done) {
        if (Human!=0) {
            // drive(STOP);
            data[0]=0;
            data[1]=0;
        } else {
            if ( DistanceSensorValue < 50) {
                // drive(LEFT);
                data[0]=20;
                data[1]=-20;
            } else {
                // drive(STRAIGHT);
                data[0]=120;
                data[1]=120;
            }
        }
        pec.publish(e);
        // mam muss nicht unbedingt warten, jedoch entlasstet dies
        // die cpu, weil es sonst busy-waiting ist
        usleep(50000);
    }
}
