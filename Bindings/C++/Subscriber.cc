/*
 * Subscriber.cc
 *
 *
 *  Michael Schulze, 2008
 */

#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

// the include contains the famouso
// configuration for local event
// propagation
#include "famouso_bindings.h"

// definition of a simple event callback that
// prints the received event data out
void cb(famouso::mw::api::SECCallBackData& cbd) {
    std::cout << __PRETTY_FUNCTION__ << " Length="
    << cbd.length << " Event data="
    << cbd.data << std::endl;
}

int main(int argc, char **argv) {

    famouso::init<famouso::config::EC>();

    // create a SubscriberEventChannel
    // with a specific Subject
    famouso::config::SEC sec(famouso::mw::Subject(0xf100000000000000ull));
//    famouso::config::SEC sec(famouso::mw::Subject("Velocity"));
    // subscribe and register the respective
    // callback that is called if an event
    // occurs
    sec.subscribe();
    sec.callback.bind<cb>();

    // busy waiting is possible, however giving up
    // the cpu is much more better for other processes
    // that have something to do
    while (1) {
        boost::xtime time;
        boost::xtime_get(&time, boost::TIME_UTC);
        time.sec += 100;
        boost::thread::sleep(time);
    }
}
