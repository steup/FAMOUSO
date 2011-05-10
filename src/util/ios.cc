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

#include "boost/thread.hpp"
#include "boost/shared_ptr.hpp"
#include "util/ios.h"
#include "debug.h"

namespace famouso {
    namespace util {
        namespace impl {
            namespace detail {

                boost::shared_ptr<boost::thread> ios_thread;
                volatile bool running=true;

                void run() {
#ifdef __XENOMAI__
                    struct sched_param param;
                    memset(&param, 0, sizeof(param));
                    param.sched_priority = 96;
                    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
#endif
                    try {
                        boost::asio::io_service::work work(famouso::util::ios::instance());
                        while(running) {
                            famouso::util::ios::instance().reset();
                            famouso::util::ios::instance().run();
                        }
                    } catch (const char *c) {
                        ::logging::log::emit< ::logging::Error>() << "Exception : " << c << ::logging::log::endl;
                        abort();
                    }
                }
            }

            void start_ios() {
                if (!detail::ios_thread)
                    detail::ios_thread.reset(new boost::thread(boost::bind(&famouso::util::impl::detail::run)));
            }

            void exit_ios() {
                if (detail::ios_thread) {
                    detail::running=false;
                    famouso::util::ios::instance().stop();
                    detail::ios_thread->join();
                    detail::ios_thread.reset();
                }
            }
        }
    }
}

