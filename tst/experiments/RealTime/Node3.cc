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

#define FAMOUSO_NODE_ID "NodeOth1"
#include "RTNodeCommon.h"
#include "eval_RTPEC.h"
#include "eval_RTSEC.h"
#include "eval_app_def.h"


#ifdef HIGH_NRT_LOAD
void subscriber_callback(const famouso::mw::api::SECCallBackData& event) {
    ::logging::log::emit() << "callback: subject " << event.subject
                           << ", length " << ::logging::log::dec << event.length
                           << ", time " << timefw::TimeSource::current()
                           << ", data " << event.data
                           << logging::log::endl;
}
#endif

int main(int argc, char ** argv) {
    famouso::init<famouso::config>(argc, argv);
    CLOCK_SYNC_INIT;

    using namespace famouso;

    // Currently we have no driver supporting parallel NRT/RT tx from one node
    // -> do only one of both
#ifdef HIGH_NRT_LOAD
    config::SEC nrt_sec("nrt_____");
    nrt_sec.callback.bind<&subscriber_callback>();
    nrt_sec.subscribe();
#else

    EvalRTPEC<
        config::PEC,
        mw::attributes::detail::SetProvider<
             mw::attributes::Period<sensor2::period>,
             mw::attributes::MaxEventLength<sensor2::mel>/*,
             mw::attributes::RealTimeSlotStartBoundary<sensor2::dt_start>,
             mw::attributes::RealTimeSlotEndBoundary<sensor2::dt_end>
             */
        >::attrSet
    > sensor2_pec("sensor_2", 0 /*sensor2::pt_start*/);
    sensor2_pec.announce();

    EvalRTSEC<
        config::SEC,
        mw::attributes::detail::SetProvider<
             mw::attributes::Period<motor2::period>,
             mw::attributes::MaxEventLength<motor2::mel>
        >::attrSet
    > motor2_sec("motor__2", 1000/*motor2::st_start*/);
    motor2_sec.subscribe();

    EvalRTPEC<
        config::PEC,
        mw::attributes::detail::SetProvider<
             mw::attributes::Period<emrgstop::period>,
             mw::attributes::MaxEventLength<emrgstop::mel>/*,
             mw::attributes::RealTimeSlotStartBoundary<emrgstop::dt_start>,
             mw::attributes::RealTimeSlotEndBoundary<emrgstop::dt_end>
             */
        >::attrSet
    > es_pec("emrgstop", 2000/*emrgstop::pt_start*/);
    es_pec.announce();
#endif

    printf("Start dispatcher\n");
    ::logging::log::emit() << "Start dispatcher\n";
    timefw::Dispatcher::instance().run();

    return 0;
}

