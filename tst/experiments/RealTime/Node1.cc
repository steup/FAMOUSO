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

#define FAMOUSO_NODE_ID "SeMoNode"
#include "RTNodeCommon.h"
#include "eval_LatRTPEC.h"
#include "eval_LatRTSEC.h"
#include "eval_app_def.h"


int main(int argc, char ** argv) {
    famouso::init<famouso::config>(argc, argv);
    CLOCK_SYNC_INIT;
    struct sched_param param;
    memset(&param, 0, sizeof(param));
    param.sched_priority = 1;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    using namespace famouso;

    EvalLatRTPEC<
        config::PEC,
        mw::attributes::detail::SetProvider<
             mw::attributes::Period<sensor1::period>,
             mw::attributes::MaxEventLength<sensor1::mel>,
             mw::attributes::RealTimeSlotStartBoundary<sensor1::dt_start>,
             mw::attributes::RealTimeSlotEndBoundary<sensor1::dt_end>
        >::attrSet
    > sensor1_pec("Sensor_1", sensor1::pt_start);
    sensor1_pec.announce();

    EvalLatRTSEC<
        config::SEC,
        mw::attributes::detail::SetProvider<
             mw::attributes::Period<motor1::period>,
             mw::attributes::MaxEventLength<motor1::mel>
        >::attrSet
    > motor1_sec("Motor__1", motor1::st_start);
    motor1_sec.subscribe();

    printf("Start dispatcher\n");
    ::logging::log::emit() << "Start dispatcher\n";
    timefw::Dispatcher::instance().run();

    return 0;
}

