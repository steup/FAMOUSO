/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *               2009-2010 Michael Kriese <kriese@cs.uni-magdeburg.de>
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
 * Publisher.cc
 *
 * Michael Schulze, 2008
 */

#include "debug.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

// the include contains the famouso
// configuration for local event
// propagation
#include "config.h"

int main(int argc, char **argv) {

    famouso::init<famouso::config>();

    famouso::config::PEC *pec = new famouso::config::PEC(famouso::mw::Subject(s));

    // announce the channel
    pec->announce();

    int pnum = 0;

    famouso::mw::Event e(pec->subject());
    e.length = sizeof(int);
    e.data = (uint8_t *)&pnum;

    while (1) {

        ::logging::log::emit() << "Packet=" << *((int *) e.data) << ::logging::log::endl;
        pec->publish(e);
        pnum++;

        boost::xtime time;
        boost::xtime_get(&time, boost::TIME_UTC_);
        time.sec += 3;
        boost::thread::sleep(time);
    }

    return 0;
}
