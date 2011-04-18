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

#ifndef __EVAL_LATRTSEC_H_563F48CC533E0B__
#define __EVAL_LATRTSEC_H_563F48CC533E0B__

#include "RealTimeSubscriberEventChannel.h"
#include "eval_LatencyDistribution.h"
#include "eval_OmissionCounter.h"

template <class SEC, class Req>
class EvalLatRTSEC : public
#if defined(RT_TEST_COM_LAT) || defined(RT_TEST_ALL_NRT)
        famouso::mw::api::RealTimeSubscriberEventChannelBase<SEC, Req>
#else
        famouso::mw::api::RealTimeSubscriberEventChannel<SEC, Req>
#endif
{
        // Subscriber task not used in all configurations
        timefw::Task sub_task;

        EvalOmissionCounter oc;
        EvalLatencyDistribution lat_dist;

    public:
#if defined(RT_TEST_COM_LAT) || defined(RT_TEST_ALL_NRT)
        typedef famouso::mw::api::RealTimeSubscriberEventChannelBase<SEC, Req> Base;
#else
        typedef famouso::mw::api::RealTimeSubscriberEventChannel<SEC, Req> Base;
#endif

        EvalLatRTSEC(const famouso::mw::Subject & subj,
                     const timefw::Time & sub_task_start) :
#if defined(RT_TEST_COM_LAT) || defined(RT_TEST_ALL_NRT)
            Base(subj)
#else
            Base(subj, sub_task_start)
#endif
        {
#if defined(RT_TEST_COM_LAT) || defined(RT_TEST_ALL_NRT)
            // Use receive callback and no periodic subscriber notify task
            Base::callback.template bind<EvalLatRTSEC, &EvalLatRTSEC::notify_latency>(this);
#else
            Base::notify_callback.template bind<EvalLatRTSEC, &EvalLatRTSEC::notify_latency>(this);
            Base::exception_callback.template bind<EvalLatRTSEC, &EvalLatRTSEC::exception>(this);
#endif
        }

        ~EvalLatRTSEC() {
#if defined(RT_TEST_STATISTICS)
            char filename[100];
            snprintf(filename, 100, "lat_%.8s.table", Base::subject().tab());
            lat_dist.log_latency_distribution(filename);
#if defined(RT_TEST_COM_LAT)
            ::logging::log::emit() << "Writing comm. lat. to " << filename << '\n';
#else
            ::logging::log::emit() << "Writing end-to-end lat. to " << filename << '\n';
            oc.log_results(Base::subject());
#endif
#endif
        }

        void notify_latency(const famouso::mw::Event & event) {
            timefw::Time recv = timefw::TimeSource::current();
            FAMOUSO_ASSERT(event.length >= 8);
            timefw::Time sent = timefw::Time::nsec(ntohll(*reinterpret_cast<uint64_t *>(event.data)));

#if defined(RT_TEST_OUTPUT_PER_PERIOD)
#if defined(RT_TEST_COM_LAT)
            ::logging::log::emit() << "[TEST_CL]: sent " << sent << " recv " << recv;
#else
            ::logging::log::emit() << "[TEST_EL]: sent " << sent << " recv " << recv;
#endif
            if (recv < sent)
                ::logging::log::emit() << " lat -" << (sent - recv).get_nsec() << "ns\n";
            else
                ::logging::log::emit() << " lat " << (recv - sent).get_nsec() << "ns\n";
#endif

#if defined(RT_TEST_STATISTICS)
            int64_t lat_ns = ((int64_t)recv.get_nsec() - (int64_t)sent.get_nsec());
            int64_t lat_us = (lat_ns + 500) / 1000;     // round

#if defined(RT_TEST_STATISTICS_NO_NRT)
            // Start recording latency after 5 minutes
            static uint64_t first_add = 0;
            if (!first_add)
                first_add = recv.get_sec() + 5 * 60;
            else if (first_add < recv.get_sec())
                lat_dist.add_latency(lat_us);
#else
            lat_dist.add_latency(lat_us);
#endif
#endif
            oc.received_event();
        }

        void exception() {
            oc.omitted_event();
#if defined(RT_TEST_OUTPUT_PER_PERIOD)
            ::logging::log::emit()
#ifdef RT_TEST_DATA_CHECK
                << "[TEST_DC] " << timefw::TimeSource::current()
#elif defined(RT_TEST_E2E_LAT)
                << "[TEST_EL] " << timefw::TimeSource::current()
#endif
                << " event omission\n";
#endif
        }
};

#endif // __EVAL_LATRTSEC_H_563F48CC533E0B__


