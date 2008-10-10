/*
 * sendEvent.c
 *  - sends distance events (Remote distance Sensor) 
 *
 *  Michael Schulze, 2008
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include "famouso_bindings.h"
#include "util/Thread.h"

int done;

void siginthandler(int egal)
{
	done = 1;
}

int main(int argc, char **argv) {
	printf("Remote distance Sensor\n");
	done = 0;
	signal(SIGINT,siginthandler);

//	PEC pec(0xf100000000000000ull);
	PEC pec(0x44697374616e6365ull);
	pec.announce();

	famouso::mw::Event e(pec.subject());
	uint8_t data[3]={1,50,'v'};
	e.length = 3;
	e.data = data;

	while(!done) {
    // 25 us ist sicherlich die untere Grenze, sonst laufen die
    // internen Queues im System ueber
    // ist auch abhaengig von der Gegenseite, ob sie die Daten
    // auch abnimmt
		usleep(50000);
    // publish data
	//	e.data[0]++;
		pec.publish(e);
	}
}



