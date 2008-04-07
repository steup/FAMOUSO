/*
 * distance.cc
 *
 *
 *  Michael Schulze, 2008
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include "famouso_binding.h"
#include "util/Thread.h"


int done;


void siginthandler(int egal)
{
	done = 1;
}

int main(int argc, char **argv) {

	SEC	sec(0xf200000000000000ll);
	Event m(sec.subject());

	done = 0;
	signal(SIGINT,siginthandler);

	sec.subscribe();

	// Integration eines Callbacks bis nach oben fehlt noch
	while(!done) {
//// mam muss nicht unbedingt warten, jedoch entlasstet dies
//// die cpu, weil es sonst busy-waiting ist
		Thread::sleep(1);
	}
}
