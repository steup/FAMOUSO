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


// Period -> real time
typedef famouso::mw::attributes::detail::SetProvider<
         famouso::mw::attributes::Period<500000>,       // 500 ms
         famouso::mw::attributes::MaxEventLength<16>    // 16 Byte -> bei CAN 3 Fragmente
        >::attrSet rt_req;

class PEC3 : public famouso::mw::api::RealTimePublisherEventChannel<famouso::config::PEC, rt_req> {
        Task pub_task;
        famouso::mw::Event event;
    public:
        typedef famouso::mw::api::RealTimePublisherEventChannel<famouso::config::PEC, rt_req> Base;

        PEC3(const famouso::mw::Subject & subj) :
            Base(subj),
            event(subj)
        {
            pub_task.start = TimeSource::current().add_usec(500000);
            pub_task.period = period;
            pub_task.function.bind<PEC3, &PEC3::publish_task>(this);
            Dispatcher::instance().enqueue(pub_task);
        }

        void publish_task() {
            event.data = (uint8_t *)"data_from_node_2";
            event.length = 16;
            publish(event);
        }
};


void subscriber_callback(const famouso::mw::api::SECCallBackData& event) {
    ::logging::log::emit() << "callback: subject " << event.subject
                           << ", length " << ::logging::log::dec << event.length
                           << ", data " << event.data
                           << ", time " << famouso::TimeSource::current()
                           << logging::log::endl;
}

int main(int argc, char ** argv) {
    famouso::init<famouso::config>();

    // can be integrated into famouso init
    famouso::config::SEC time_chan("TimeSync");
    typedef famouso::TimeSource::clock_type Clock;
    time_chan.callback.bind<Clock, &Clock::tick>(&famouso::TimeSource::instance());
    time_chan.subscribe();

    while (famouso::TimeSource::out_of_sync()) {
        usleep(1000);
    }
    printf("Clock in sync\n");


    famouso::config::SEC sec1("rt_____1");
    sec1.callback.bind<&subscriber_callback>();
    sec1.subscribe();

//    {
    PEC3 pec2("rt_____0");
    pec2.announce();
//    }

    printf("Start dispatcher\n");
    Dispatcher::instance().run();

    return 0;
}

