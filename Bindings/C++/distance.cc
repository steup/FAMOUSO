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

#include "famouso_bindings.h"
#include "util/Thread.h"


int done;

void cb(famouso::mw::api::SECCallBackData& cbd) {
  printf("%s Parameter=%d Daten:=%s\n", __PRETTY_FUNCTION__, cbd.length, cbd.data);
}


void siginthandler(int egal)
{
	done = 1;
}

int main(int argc, char **argv) {

	SEC	sec(0xf200000000000000ll);
	famouso::mw::Event m(sec.subject());

	done = 0;
	signal(SIGINT,siginthandler);

	sec.subscribe();
	sec.callback.bind<cb>();

	// Integration eines Callbacks bis nach oben fehlt noch
	while(!done) {
//// mam muss nicht unbedingt warten, jedoch entlasstet dies
//// die cpu, weil es sonst busy-waiting ist
		Thread::sleep(100);
	}
}
