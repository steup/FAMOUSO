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

#include "mw/afp/shared/Time.h"
#include "case/Delegate.h"
#include "object/Queue.h"

#include <signal.h>

volatile bool ___done = false;

void siginthandler(int) {
    // Unlock the idle() thread
    ___done=true;
}



struct Task : public Chain {
    Time start;
    uint64_t period;                        // in usec, Zero for non-periodic
    famouso::util::Delegate<void> function; // Later we need argument binding
};

// little dispatcher sketch for first testing while waiting for real dispatcher implementation
class Dispatcher {
        Queue tasks;

        Task * next_task() {
            // Returns first task in timeline
            Task * earliest = 0;
            Task * task = static_cast<Task*>(tasks.select());
            while (task) {
                if (!earliest || task->start < earliest->start) {
                    earliest = task;
                }
                task = static_cast<Task*>(task->select());
            }
            return earliest;
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

        static Dispatcher & instance() {
            static Dispatcher d;
            return d;
        }

        void enqueue(Task & task) {
            tasks.append(task);
        }

        void dequeue(Task & task) {
            tasks.remove(task);
        }

        // No real time!!!
        void run() {
            signal(SIGINT, siginthandler);
            while (!___done) {
                Task * next = next_task();
                if (next) {
                    Time curr = Time/*Source*/::current();
                    if (next->start < curr) {
                        ::logging::log::emit<Dispatcher>() << "task scheduled for " << next->start << " started " << curr << "\n";
                        // Run task
                        next->function();
                        if (next->period == 0) {
                            dequeue(*next);
                        } else {
                            next->start.add_usec(next->period);
                            //next->start = Time::current().add_usec(next->period);
                        }
                    }
                } else {
                    usleep(100);
                }
            }
        }
};

#endif // __DISPATCHER_H_65D8DDCDE85B34__

