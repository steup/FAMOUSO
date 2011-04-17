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

#ifndef __EVAL_LATRTPEC_H_2192A365C65E77__
#define __EVAL_LATRTPEC_H_2192A365C65E77__

#include "RealTimePublisherEventChannel.h"

template <class PEC, class Req>
class EvalLatRTPEC : public famouso::mw::api::RealTimePublisherEventChannel<PEC, Req> {
        famouso::mw::Event event;

        typedef famouso::mw::api::RealTimePublisherEventChannel<PEC, Req> Base;
    public:

        EvalLatRTPEC(const famouso::mw::Subject & subj,
                  const timefw::Time & pub_task_start) :
            Base(subj, pub_task_start),
            event(subj)
        {
#if defined(RT_TEST_COM_LAT)
            Base::deliver_task.template bind<EvalLatRTPEC, &EvalLatRTPEC::publish_task_func>(this);
#else
            Base::publisher_task.template bind<EvalLatRTPEC, &EvalLatRTPEC::publish_task_func>(this);
#endif
        }

        void publish_task_func() {
            timefw::Time curr = timefw::TimeSource::current().get();
            uint8_t buffer[Base::mel];
            memset(buffer, 0, Base::mel);
            FAMOUSO_ASSERT(Base::mel >= 8);
            *reinterpret_cast<uint64_t*>(buffer) = htonll(curr.get());
            event.length = Base::mel;
            event.data = buffer;
#if defined(RT_TEST_COM_LAT)
            Base::deliver_to_net(event);
#else
            Base::publish(event);
#endif
        }
};

#endif // __EVAL_LATRTPEC_H_2192A365C65E77__


