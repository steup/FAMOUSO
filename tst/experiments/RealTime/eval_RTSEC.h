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

#ifndef __EVAL_RTSEC_H_B89BB49F2E2578__
#define __EVAL_RTSEC_H_B89BB49F2E2578__

#include "mw/api/RealTimeSubscriberEventChannel.h"
#include "eval_OmissionCounter.h"

template <class SEC, class Req>
class EvalRTSEC : public famouso::mw::api::RealTimeSubscriberEventChannel<SEC, Req> {
        uint64_t counter;

#ifdef RT_TEST_STATISTICS
        EvalOmissionCounter oc;
#endif

    public:
        typedef famouso::mw::api::RealTimeSubscriberEventChannel<SEC, Req> Base;

        EvalRTSEC(const famouso::mw::Subject & subj,
                  const timefw::Time & sub_task_start) :
            Base(subj)
#ifdef RT_TEST_STATISTICS
            , counter(0)
#endif
        {
            Base::notify_callback.template bind<EvalRTSEC, &EvalRTSEC::notify_data_check>(this);
            Base::exception_callback.template bind<EvalRTSEC, &EvalRTSEC::exception>(this);
        }

        ~EvalRTSEC() {
#ifdef RT_TEST_STATISTICS
            oc.log_results(Base::subject());
#endif
        }

        void notify_data_check(const famouso::mw::Event & event) {
#ifdef RT_TEST_STATISTICS
            oc.received_event();
#endif
#if defined(RT_TEST_OUTPUT_PER_PERIOD)
            ::logging::log::emit()
                << "[TEST_DC] " << timefw::TimeSource::current()
                << " event received\n";
#endif

            if (Base::mel == 0)
                return;

            // Expect sequence number (if event.length > 8 the rest is zeroed)
            uint64_t rcv_counter = 0;
            unsigned int it_end = event.length < 8 ? event.length : 8;
            for (unsigned int i = 0; i < it_end; ++i) {
                rcv_counter <<= 8;
                rcv_counter |= event.data[i];
            }
            for (unsigned int i = it_end; i < event.length; ++i) {
                if (event.data[i] != 0) {
                    ::logging::log::emit()
                        << "[TEST_DC] " << timefw::TimeSource::current()
                        << " Unexpected event content\n";
                    break;
                }
            }
            if (counter != rcv_counter) {
                ::logging::log::emit()
                    << "[TEST_DC] " << timefw::TimeSource::current()
                    << " expected seq " << counter
                    << " received seq " << rcv_counter << " on chan " << Base::subject() << '\n';
                counter = rcv_counter;
            }
            ++counter;
        }

        void exception() {
#ifdef RT_TEST_STATISTICS
            oc.omitted_event();
#endif
#if defined(RT_TEST_OUTPUT_PER_PERIOD)
            ::logging::log::emit()
                << "[TEST_DC] " << timefw::TimeSource::current()
                << " event omission\n";
#endif
        }
};


#endif // __EVAL_RTSEC_H_B89BB49F2E2578__


