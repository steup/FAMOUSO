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


#ifndef __DISPATCHER_H_65D8DDCDE85B34__
#define __DISPATCHER_H_65D8DDCDE85B34__

#include "case/Delegate.h"
#include "object/Queue.h"
#include "util/ios.h"
#include "util/math.h"

#include "timefw/Time.h"
#include "timefw/Singleton.h"
#include <signal.h>


namespace timefw {

    volatile bool ___done = false;

    void siginthandler(int) {
        // Unlock the idle() thread
        ___done=true;
    }



    // Later we need argument binding (multiple Deliver-Tasks on one RT-PEC)
    struct Task : public famouso::util::Delegate<void>, public Chain {
        bool realtime;
        Time start;
        uint64_t period;                        // in usec, Zero for non-periodic
        Task() : realtime(false) {}
        Task(const Time & s, uint64_t p = 0, bool rt = false) : realtime(rt), start(s), period(p) {}
    };

    // simple function dispatcher sketch for first testing
    // No real time scheduling, but best effort function dispatching assuming enough CPU ressources
    class DispatcherImpl {
            Queue tasks;

            void insert_task(Task & new_task) {
                // Sort list for starting time
                Chain * predecessor = &tasks;
                Task * task = static_cast<Task*>(tasks.select());
                while (task && task->start < new_task.start) {
                    predecessor = task;
                    task = static_cast<Task*>(task->select());
                }
                new_task.select(task);
                predecessor->select(&new_task);
            }

            void remove_task(Task & task) {
                tasks.remove(&task);
            }

            void wait_for_next_task() {
                // TODO: wenn nebenläufiges ändern von tasks (z.b. in callbacks während dispatcher läuft): warten ggf. abzubrechen
                Task * task = static_cast<Task*>(tasks.select());
                if (task) {
                    TimeSource::wait_until(task->start);
                } else {
                    // No tasks
                    usleep(500000);
                }
            }

            void dispatch(Task & task, const Time & curr) {
                // Run task
                task();
                // Insert periodic task into queue again
                if (task.period != 0) {
                    // Ensure to increase starting time by one period
                    task.start.add_usec(task.period);
                    // Ensure to starting time is in the future
                    task.start.set(increase_by_multiple_above(task.start.get(), task.period, curr.get()));
                    insert_task(task);
                }
            }

        public:

            // Nicht 2x enqueuen
            // Nicht nachträglich zu ändern!
            void enqueue(Task & task) {
#ifdef DISPATCHER_OUTPUT
                ::logging::log::emit() << "[DISPATCH] enqueued task scheduled for " << task.start << " (currtly " << timefw::TimeSource::current() << ')' << '\n';
#endif
                if (task.period)
                    task.start.set(increase_by_multiple_above(task.start.get(), task.period, timefw::TimeSource::current().get()));
                insert_task(task);
            }

            void dequeue(Task & task) {
                remove_task(task);
            }

            void run() {
                signal(SIGINT, siginthandler);
                signal(SIGHUP, siginthandler);
                signal(SIGQUIT,siginthandler);
                signal(SIGTERM,siginthandler);
                signal(SIGPIPE,siginthandler);

                wait_for_next_task();
                while (!___done) {
                    Task * next = static_cast<Task *>(tasks.unlink());
                    if (next) {
                        Time curr = TimeSource::current();
#ifdef DISPATCHER_OUTPUT
                        ::logging::log::emit() << "[DISPATCH] task scheduled for " << next->start << " started " << curr << '\n';
#endif
                        // Run task
                        dispatch(*next, curr);
                    }
                    wait_for_next_task();
//                    ::logging::log::emit() << '.';
                }
                // signalise the ios to exit
                famouso::util::impl::exit_ios();
            }

            /// Should be called by NRT tasks to when waiting for ressources (cooperative flow transfer to RT tasks)
            void yield_for_rt() {
                Task * next;
                while (1) {
                    // Look for next real time task
                    do {
                        next = static_cast<Task *>(tasks.unlink());
                        if (!next)
                            return;
                    } while (!next->realtime);

                    // Check if it should have been started
                    Time curr = TimeSource::current();
                    if (curr < next->start)
                        return;

                    // Task start time not in future -> run task
#ifdef DISPATCHER_OUTPUT
                    ::logging::log::emit() << "[DISPATCH] task scheduled for " << next->start << " started " << curr << " (NRT yielded for RT)\n";
#endif
                    dispatch(*next, curr);
                }
            }
    };

    // TODO: maybe static + init in famouso-init is sufficient (save if)
    typedef Singleton<DispatcherImpl> Dispatcher;

} // namespace timefw


#endif // __DISPATCHER_H_65D8DDCDE85B34__

