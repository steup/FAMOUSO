/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
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

#define FAMOUSO_NODE_ID "RT_Node2"
#include "RTNodeCommon.h"
#include "evaluation.h"


void subscriber_callback(const famouso::mw::api::SECCallBackData& event) {
    ::logging::log::emit() << "callback: subject " << event.subject
                           << ", length " << ::logging::log::dec << event.length
                           << ", data " << event.data
                           << ", time " << timefw::TimeSource::current()
                           << logging::log::endl;
}

int main(int argc, char ** argv) {
    famouso::init<famouso::config>();
    CLOCK_SYNC_INIT;


    /*
    famouso::config::SEC sec1("rt_____0");
    sec1.callback.bind<&subscriber_callback>();
    sec1.subscribe();

    famouso::config::PEC pec1("nrt____0");
    pec1.announce();
    */

//    {
    // PEC QoS
    const uint64_t period = 200000;        // 200 ms
    const uint64_t mel = 70;//17;               // 17 Byte -> bei CAN 3 Fragmente

    // Local CPU schedule
    const uint64_t subscribe_start_time = period / 2;

    typedef famouso::mw::attributes::detail::SetProvider<
             famouso::mw::attributes::Period<period>,
             famouso::mw::attributes::MaxEventLength<mel>
            >::attrSet Req;

    TestRTSEC<famouso::config::SEC, Req> sec("rt1_(n1)", subscribe_start_time);
    sec.subscribe();
//    }

    printf("Start dispatcher\n");
    timefw::Dispatcher::instance().run();

    return 0;
}

