/*******************************************************************************
 *
 * Copyright (c) 2011 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#include "debug.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

// the include contains the famouso
// configuration for local event
// propagation
#include "../Bindings/include/famouso_bindings.h"

int main(int argc, char **argv) {

    if (argc<3) {
        logging::log::emit()
            << "usage:"
            << logging::log::endl
            << logging::log::tab
            << argv[0] << " msglength time"
            << logging::log::endl
            << logging::log::tab
            << "msglength in bytes, possible values are 0-8"
            << logging::log::endl
            << logging::log::tab
            << "time in ms, possible values are  0-250"
            << logging::log::endl;
        return -1;
    }

    famouso::init<famouso::config>();

    // create a PublisherEventChannel
    // with a specific Subject
    famouso::config::PEC pec(famouso::mw::Subject("EvalConf"));

    // announce the channel
    pec.announce();

    famouso::mw::Event e(pec.subject());
    e.length = 2;
    uint8_t data[2];
    data[0]=(uint8_t)atoi(argv[1]);
    data[1]=(uint8_t)atoi(argv[2]);
    e.data = data;

    pec.publish(e);
}
