/*
 * Publisher.cc
 *
 * Michael Schulze, 2008
 */

#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

// the include contains the famouso
// configuration for local event
// propagation
#include "famouso_bindings.h"

int main(int argc, char **argv) {

    famouso::init<famouso::config::EC>();

    // create a PublisherEventChannel
    // with a specific Subject
    famouso::config::PEC pec(0xf100000000000000ull);
    // announce the channel
    pec.announce();

    famouso::mw::Event e(pec.subject());
    e.length = 7;
    e.data = (uint8_t*)"Publish";

    // busy waiting is possible, however giving up
    // the cpu is much more better for other processes
    // that have something to do
    while (1) {
        pec.publish(e);
        boost::xtime time;
        boost::xtime_get( &time, boost::TIME_UTC );
        time.sec += 1;
        boost::thread::sleep( time );
    }
}
