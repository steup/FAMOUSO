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

#ifndef __EVAL_RTPEC_H_D03B33C2AEFBC7__
#define __EVAL_RTPEC_H_D03B33C2AEFBC7__

#ifndef RT_TEST_ALL_NRT
#include "mw/api/RealTimePublisherEventChannel.h"
#endif
#include "AddPublisherTask.h"
#include "eval_ProvideSimpleAttribAccess.h"

template <class PEC, class Req>
class EvalRTPEC :
    public AddPublisherTask<
#ifndef RT_TEST_ALL_NRT
        famouso::mw::api::RealTimePublisherEventChannel<PEC, Req>
#else
        ProvideSimpleAttribAccess<PEC, Req>
#endif
    >
{
        famouso::mw::Event event;
        uint64_t counter;

#ifndef RT_TEST_ALL_NRT
        typedef AddPublisherTask<
            famouso::mw::api::RealTimePublisherEventChannel<PEC, Req> > Base;
#else
        typedef AddPublisherTask<
            ProvideSimpleAttribAccess<PEC, Req> > Base;
#endif
    public:

        EvalRTPEC(const famouso::mw::Subject & subj,
                  const timefw::Time & pub_task_start) :
            Base(subj, pub_task_start),
            event(subj),
            counter(0)
        {
            Base::publisher_task.template bind<EvalRTPEC, &EvalRTPEC::publish_task_func>(this);
        }

        void publish_task_func() {
            if (Base::mel) {
                uint8_t buffer[Base::mel];
                memset(buffer, 0, Base::mel);
                // Tx sequence number (if MaxEventLength > 8 the rest is zeroed)
                uint64_t tmp = counter;
                unsigned int last = Base::mel <= 8 ? Base::mel-1 : 7;
                for (int i = last; i >= 0 && tmp; --i) {
                    buffer[i] = tmp & 0xff;
                    tmp >>= 8;
                }
                event.data = buffer;
                event.length = Base::mel;
                Base::publish(event);
            } else {
                event.data = (uint8_t*)"";
                event.length = Base::mel;
                Base::publish(event);
            }
            ++counter;
        }
};

#endif // __EVAL_RTPEC_H_D03B33C2AEFBC7__


