/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#include "mw/nl/can/etagBP/PreventBlockingOfMiddlewareCoreInBlockingProtocol.h"

#include "util/ios.h"
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/detail/singleton.hpp>
#include <boost/thread/xtime.hpp>
#ifdef __XENOMAI__
#include <time.h>
#endif

#include <stdio.h>
namespace famouso {
    namespace mw {
        namespace nl {
            namespace CAN {
                namespace etagBP {

                    static void sleep() {
#ifdef __XENOMAI__
                        // boost::thread::sleep() does not behave correctly in XENOMAI
                        timespec time;
                        time.tv_sec = 0;
                        time.tv_nsec = 100000000LLU;
                        clock_nanosleep(CLOCK_REALTIME, 0, &time, 0);
#else
                        boost::xtime time;
#if BOOST_VERSION >= 105000
                        boost::xtime_get(&time, boost::TIME_UTC_);
#else
                        boost::xtime_get(&time, boost::TIME_UTC);
#endif
                        time.nsec += 10000000;
                        boost::thread::sleep(time);
#endif
                    }



                    typedef boost::detail::thread::singleton<boost::mutex> _mutex;

                    void PreventBlockingOfMiddlewareCoreInBlockingProtocol::runMiddlewareCore() {
                        while (!passed) {
                            famouso::util::ios::instance().poll();
                            sleep();
                        }
                    }

                    // launch a thread that lead the MiddlewareCore  proceed
                    PreventBlockingOfMiddlewareCoreInBlockingProtocol::PreventBlockingOfMiddlewareCoreInBlockingProtocol() :
                            passed(false) {
                        thrd = new boost::thread(boost::bind(&PreventBlockingOfMiddlewareCoreInBlockingProtocol::runMiddlewareCore, this));
                        _mutex::instance().lock();
                    }

                    PreventBlockingOfMiddlewareCoreInBlockingProtocol::~PreventBlockingOfMiddlewareCoreInBlockingProtocol() {
                        passed = true;
                        _mutex::instance().unlock();
                        reinterpret_cast<boost::thread*>(thrd)->join();
                        delete reinterpret_cast<boost::thread*>(thrd);
                    }

                    void PreventBlockingOfMiddlewareCoreInBlockingProtocol::process() {
                        // nothing to do here except sleeping, because the middleware
                        // core is executed in another thread
                        sleep();
                    }

                }
            } /* namespace CAN */
        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */

