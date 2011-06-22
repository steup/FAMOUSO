/*******************************************************************************
 *
 * Copyright (c) 2011 Philipp Werner <philipp.werner@st.ovgu.de>
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

#define FAMOUSO_NODE_ID "MasterNo"
#define USE_LOCAL_CLOCK
#define BE_CAN_BROKER
#include "RTNodeCommon.h"
#include "RTNetScheduler.h"
#include "TimeMaster.h"

// for NRT-Polling test only
void subscriber_callback(const famouso::mw::api::SECCallBackData& event) {
    ::logging::log::emit() << "callback: subject " << event.subject
                           << ", length " << ::logging::log::dec << event.length
                           << ", time " << timefw::TimeSource::current()
                           << ", data " << event.data
                           << logging::log::endl;
}

int main(int argc, char ** argv) {
    famouso::init<famouso::config>();

    using namespace famouso::mw::rt_net_sched;
    RTNetScheduler<famouso::config::PEC, famouso::config::SEC> sched;
#ifndef __ETHERNET__
    NetworkSchedule can(famouso::mw::el::ml::NetworkID(/*"CAN0@1Mb"*/(uint64_t)0),
                        CanNetworkTimingConfig(
                            250000,     // Bits per second
                            50,        // Uhrengranularität in us
                            1000,       // Planungsgranularität in us
                            //100,       // Planungsgranularität in us
                            10000,      // Trigger bis USF in nano sec
                            10000       // USF bis auf Medium in nano sec
                        ),
                        sched);
#else
    NetworkSchedule eth(famouso::mw::el::ml::NetworkID(/*"CAN0@1Mb"*/(uint64_t)0),
                        EthernetNetworkTimingConfig(
                            100 * 1000000,     // Bits per second
                            50,        // Uhrengranularität in us
                            1000,       // Planungsgranularität in us
                            //100,       // Planungsgranularität in us
                            10000,      // Trigger bis USF in nano sec
                            10000       // USF bis auf Medium in nano sec
                        ),
                        sched,
                        true);          // PollMaster
    famouso::mw::poll_master::PollMaster<famouso::config::PEC, famouso::config::SEC> poll_m(
                            1000,       // Planungsgranularität in us
                            5000,       // Overhead
                            10000       // Minimum poll slot length in us
                        );
    poll_m.push_back(getNodeID<void>());
    poll_m.push_back(NodeID("SeMoNode"));
    poll_m.push_back(NodeID("CtrlNode"));
    poll_m.push_back(NodeID("Oth1Node"));
    poll_m.push_back(NodeID("Oth2Node"));
#endif

    TimeMaster<famouso::config::PEC> time_master;

    // for NRT-Polling test only
    famouso::config::SEC nrt_sec("nrt_____");
    nrt_sec.callback.bind<&subscriber_callback>();
    nrt_sec.subscribe();

    ::logging::log::emit() << "Start dispatcher\n";
    timefw::Dispatcher::instance().run();
    ::logging::log::emit() << "Stop dispatcher\n";

    return 0;
}

