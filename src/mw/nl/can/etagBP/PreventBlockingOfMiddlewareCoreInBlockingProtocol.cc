/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
#include <boost/pool/detail/singleton.hpp>

namespace famouso {
    namespace mw {
        namespace nl {
            namespace CAN {
                namespace etagBP {

                    typedef boost::details::pool::singleton_default<boost::mutex> _mutex;

                    void PreventBlockingOfMiddlewareCoreInBlockingProtocol::runMiddlewareCore() {
                        while (!passed) {
                            famouso::util::ios::instance().poll();
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

                }
            } /* namespace CAN */
        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */

