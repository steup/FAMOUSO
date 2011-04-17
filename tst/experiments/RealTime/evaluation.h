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

#ifndef __EVALUATION_H_5AB699BB24F373__
#define __EVALUATION_H_5AB699BB24F373__

#include "RealTimePublisherEventChannel.h"
#include "RealTimeSubscriberEventChannel.h"

template <class PEC, class Req>
class TestRTPEC : public famouso::mw::api::RealTimePublisherEventChannel<PEC, Req> {
        timefw::Task pub_task;
        famouso::mw::Event event;
        uint64_t counter;
    public:
        typedef famouso::mw::api::RealTimePublisherEventChannel<PEC, Req> Base;

        TestRTPEC(const famouso::mw::Subject & subj,
                  const timefw::Time & pub_task_start) :
            Base(subj),
            event(subj),
            counter(0)
        {
            pub_task.start = increase_by_multiple_above(pub_task_start.get(), (uint64_t)Base::period, timefw::TimeSource::current().get());
            pub_task.period = Base::period;
            pub_task.bind<TestRTPEC, &TestRTPEC::publish_task_func>(this);
            timefw::Dispatcher::instance().enqueue(pub_task);
        }

        void publish_task_func() {
            uint8_t buffer[Base::mel];
            memset(buffer, 0, Base::mel);
#ifdef RT_TEST_DATA_CHECK
            // Tx sequence number (if MaxEventLength > 8 the rest is zeroed)
            uint64_t tmp = counter;
            for (unsigned int i = 0;
                     i < Base::mel && tmp;
                     ++i, tmp >>= 8)
                buffer[i] = tmp & 0xff;
            event.length = Base::mel;
            ++counter;
#elif defined(RT_TEST_E2E_LAT)
            FAMOUSO_ASSERT(Base::mel >= 8);
            *reinterpret_cast<uint64_t*>(buffer) = htonll(timefw::TimeSource::current().get());
            event.length = Base::mel;
#endif
            event.data = buffer;
            Base::publish(event);
        }
};

template <class SEC, class Req>
class TestRTSEC : public famouso::mw::api::RealTimeSubscriberEventChannel<SEC, Req> {
        timefw::Task sub_task;
        uint64_t counter;
    public:
        typedef famouso::mw::api::RealTimeSubscriberEventChannel<SEC, Req> Base;

        TestRTSEC(const famouso::mw::Subject & subj,
                  const timefw::Time & sub_task_start) :
            Base(subj),
            counter(0)
        {
#if defined(RT_TEST_DATA_CHECK)
            Base::notify_callback.template bind<TestRTSEC, &TestRTSEC::notify_data_check>(this);
#elif defined(RT_TEST_E2E_LAT)
            Base::notify_callback.template bind<TestRTSEC, &TestRTSEC::notify_latency>(this);
#elif defined(RT_TEST_COM_LAT)
            // Use receive callback and no periodic subscriber notify task
            Base::callback.template bind<TestRTSEC, &TestRTSEC::notify_latency>(this);
#endif

#if !defined(RT_TEST_COM_LAT)
            Base::exception_callback.template bind<TestRTSEC, &TestRTSEC::exception>(this);

            sub_task.start = increase_by_multiple_above(sub_task_start.get(), (uint64_t)Base::period, timefw::TimeSource::current().get());
            sub_task.period = Base::period;
            sub_task.template bind<Base, &Base::notify_task>(this);
            timefw::Dispatcher::instance().enqueue(sub_task);
#endif
        }

        void notify_data_check(const famouso::mw::Event & event) {
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
                    << " received seq " << rcv_counter << '\n';
                if (counter < rcv_counter)
                    counter = rcv_counter;
            }
            ++counter;
        }

        void notify_latency(const famouso::mw::Event & event) {
            FAMOUSO_ASSERT(event.length >= 8);
            timefw::Time recv = timefw::TimeSource::current();
            timefw::Time sent = ntohll(*reinterpret_cast<uint64_t *>(event.data));

#if defined(RT_TEST_E2E_LAT)
            ::logging::log::emit() << "[TEST_EL]: sent " << sent << " recv " << recv;
#elif defined(RT_TEST_COM_LAT)
            ::logging::log::emit() << "[TEST_CL]: sent " << sent << " recv " << recv;
#endif
            if (recv < sent)
                ::logging::log::emit() << " lat -" << sent.get() - recv.get() << "us\n";
            else
                ::logging::log::emit() << " lat " << recv.get() - sent.get() << "us\n";
        }

        void exception() {
            ::logging::log::emit()
#ifdef RT_TEST_DATA_CHECK
                << "[TEST_DC] " << timefw::TimeSource::current()
#elif defined(RT_TEST_E2E_LAT)
                << "[TEST_EL] " << timefw::TimeSource::current()
#endif
                << " event omission\n";
        }
};

#endif // __EVALUATION_H_5AB699BB24F373__

