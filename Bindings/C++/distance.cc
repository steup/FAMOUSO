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

#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

int done;

void cb(famouso::mw::api::SECCallBackData& cbd) {
    printf("%s Parameter=%d Daten:=%s\n", __PRETTY_FUNCTION__, cbd.length, cbd.data);
}

void siginthandler(int egal) {
    done = 1;
}

int main(int argc, char **argv) {
    famouso::init<famouso::config::EC>();
    famouso::config::SEC sec(famouso::mw::Subject(0x44697374616e6365ull));
    famouso::mw::Event m(sec.subject());

    done = 0;
    signal(SIGINT, siginthandler);

    sec.subscribe();
    sec.callback.bind<cb>();

    // Integration eines Callbacks bis nach oben fehlt noch
    while (!done) {
        // mam muss nicht unbedingt warten, jedoch entlasstet dies
        // die cpu, weil es sonst busy-waiting ist
        boost::xtime time;
        boost::xtime_get( &time, boost::TIME_UTC );
        time.sec += 1;
        boost::thread::sleep( time );
    }
}
