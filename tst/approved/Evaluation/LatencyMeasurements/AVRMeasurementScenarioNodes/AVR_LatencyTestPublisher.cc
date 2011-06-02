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
#include "CommonIncludes.h"
#include "mw/api/RealTimePublisherEventChannel.h"

volatile uint8_t length=8;
volatile uint8_t time=100;
void cb( famouso::mw::api::SECCallBackData& cbd) {
    length=cbd.data[0];
    time=cbd.data[1];
}

template<typename P, typename R>
struct PEC : famouso::mw::api::RealTimePublisherEventChannel< P, R > {
    PEC(const famouso::mw::Subject& subject) : famouso::mw::api::RealTimePublisherEventChannel<P,R>(subject) {}
    void deliver_to_net(const famouso::mw::Event & e) {
        famouso::mw::api::RealTimePublisherEventChannel<P,R>::deliver_to_net(e);
    }
};

int main() {

    sei();
    famouso::init<famouso::config>();
    using namespace famouso;

    PEC<
        config::PEC,
        mw::attributes::detail::SetProvider<
             mw::attributes::Period<100000>,
             mw::attributes::MaxEventLength<200>
        >::attrSet
    > sensor1_pec("Sensor_1");
    sensor1_pec.announce();


    famouso::mw::Event e(sensor1_pec.subject());
    uint8_t data[255];
    memset(data,0,255);
    e.data=data;
    e.length=8;

    config::SEC sec("EvalConf");
    sec.subscribe();
    sec.callback.bind<cb>();
    DDRD=1<<7;
    while(1){
        e.length=length;
        sensor1_pec.publish(e);
        PIND=1<<7;
        sensor1_pec.deliver_to_net(e);
        delay_ms(time);
    }

    return 0;
}

