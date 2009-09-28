/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/

/*
 * Subscriber.cc
 *
 *
 *  Michael Schulze, 2008
 */

#include "debug.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

// the include contains the famouso
// configuration for local event
// propagation
#include "famouso_bindings.h"

// definition of a simple event callback that
// prints the received event data out
void cb(famouso::mw::api::SECCallBackData& cbd) {
    log::emit() << FUNCTION_SIGNATURE << " Length="
    << cbd.length << " Event data="
    << cbd.data << log::endl;
    std::cout<< cbd.data <<std::endl;
}

int main(int argc, char **argv) {

    famouso::init<famouso::config>();

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
