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

#define FAMOUSO_NODE_ID "Oth2Node"
#include "RTNodeCommon.h"
#include "eval_RTPEC.h"
#include "eval_RTSEC.h"
#include "eval_app_def.h"


#ifdef HIGH_NRT_LOAD
famouso::config::PEC nrt_pec("nrt_____");

void nrt_publish_func() {
    timefw::Time curr = timefw::TimeSource::current();
    unsigned long long ms = curr.get() / 1000;
    unsigned int us = curr.get() % 1000;

    famouso::mw::Event event(nrt_pec.subject());
    char buffer [160];
    static int i = 0;
    if (++i % 5 == 0)
        event.length = snprintf(buffer, 160, "This is some NRT message published at (%llu ms, %u us) content to create high load on the transmission medium.", ms, us) + 1;
    else
        event.length = snprintf(buffer, 160, "1234567") + 1;
    event.data = (uint8_t*)buffer;

    nrt_pec.publish(event);
}
#endif


int main(int argc, char ** argv) {
    famouso::init<famouso::config>(argc, argv);
    CLOCK_SYNC_INIT;

    using namespace famouso;

    // Currently we have no driver supporting parallel NRT/RT tx from one node
    // -> do only one of both
#ifdef HIGH_NRT_LOAD
    timefw::Task nrt_task(0, 1, false);
    nrt_task.bind<&nrt_publish_func>();
    timefw::Dispatcher::instance().enqueue(nrt_task);
    nrt_pec.announce();
#else

    EvalRTPEC<
        config::PEC,
        mw::attributes::detail::SetProvider<
             mw::attributes::Period<motor2::period>,
             mw::attributes::MaxEventLength<motor2::mel>/*,
             mw::attributes::RealTimeSlotStartBoundary<motor2::dt_start>,
             mw::attributes::RealTimeSlotEndBoundary<motor2::dt_end>
             */
        >::attrSet
    > motor2_pec("Motor__2", timefw::Time::usec(0/*motor2::pt_start*/));
    motor2_pec.announce();

    EvalRTSEC<
        config::SEC,
        mw::attributes::detail::SetProvider<
             mw::attributes::Period<sensor2::period>,
             mw::attributes::MaxEventLength<sensor2::mel>
        >::attrSet
    > sensor2_sec("Sensor_2", timefw::Time::usec(1000/*sensor2::st_start*/));
    sensor2_sec.subscribe();

    EvalRTSEC<
        config::SEC,
        mw::attributes::detail::SetProvider<
             mw::attributes::Period<emrgstop::period>,
             mw::attributes::MaxEventLength<emrgstop::mel>
        >::attrSet
    > es_sec("Not-Aus_", timefw::Time::usec(200/*emrgstop::st_start*/));
    es_sec.subscribe();

    EvalRTPEC<
        config::PEC,
        mw::attributes::detail::SetProvider<
             mw::attributes::Period<5*1000>,
#ifdef __ETHERNET__
             mw::attributes::MaxEventLength<500>
#else
             mw::attributes::MaxEventLength<20>
#endif
        >::attrSet
    > sensor3_pec("Sensor_3", timefw::Time::usec(0));
    sensor3_pec.announce();
#endif

    printf("Start dispatcher\n");
    ::logging::log::emit() << "Start dispatcher\n";
    timefw::Dispatcher::instance().run();

    return 0;
}

