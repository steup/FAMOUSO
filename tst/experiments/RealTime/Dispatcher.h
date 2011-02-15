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

#include "Singleton.h"
#include "TFW.h"
#include "case/Delegate.h"
#include "object/Queue.h"

#include "util/ios.h"
#include <signal.h>

using famouso::TimeSource;

volatile bool ___done = false;

void siginthandler(int) {
    // Unlock the idle() thread
    ___done=true;
}



struct Task : public Chain {
    bool realtime;
    famouso::time::Time start;
    uint64_t period;                        // in usec, Zero for non-periodic
    famouso::util::Delegate<void> function; // Later we need argument binding

    Task() : realtime(false) {}
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

        void dispatch(Task & task) {
            // Run task
            task.function();
            // Insert periodic task into queue again
            if (task.period != 0) {
                task.start.add_usec(task.period);
                insert_task(task);
            }
        }

    public:

        /*! \brief delivers the current %level of %logging */
        static ::logging::Level::levels level() {
            return ::logging::Level::user;
        }
        /*! \brief delivers the string reporting the current %level of %logging */
        static const char * desc() {
            return "[DISPATCH] ";
        }

        // Nicht 2x enqueuen
        // Nicht nachträglich zu ändern!
        // TODO: prüfen ob zeit in zukunft?
        void enqueue(Task & task) {
            insert_task(task);
        }

        void dequeue(Task & task) {
            remove_task(task);
        }

        void run() {
            signal(SIGINT, siginthandler);
            while (!___done) {
                wait_for_next_task();
                Task * next = static_cast<Task *>(tasks.unlink());
                if (next) {
                    famouso::time::Time curr = TimeSource::current();
                    //::logging::log::emit<DispatcherImpl>() << "task scheduled for " << next->start << " started " << curr << "\n";
                    // Run task
                    dispatch(*next);
                }
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
                famouso::time::Time curr = TimeSource::current();
                if (curr < next->start)
                    return;

                // Task start time not in future -> run task
                ::logging::log::emit<DispatcherImpl>() << "task scheduled for " << next->start << " started " << curr << " (NRT yielded for RT)\n";
                dispatch(*next);
            }
        }
};

// TODO: maybe static + init in famouso-init is sufficient (save if)
typedef Singleton<DispatcherImpl> Dispatcher;


#endif // __DISPATCHER_H_65D8DDCDE85B34__

