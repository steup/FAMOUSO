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

#ifndef __PreventBlockingOfMiddlewareCoreInBlockingProtocol_h__
#define __PreventBlockingOfMiddlewareCoreInBlockingProtocol_h__

#include <thread>
#include <mutex>

namespace famouso {
    namespace mw {
        namespace nl {
            namespace CAN {
                namespace etagBP {

                    /*! \brief PreventBlockingOfMiddlewareCoreInBlockingProtocol
                     *         implements a class that runs the  middleware
                     *         core if we go to a blocking protocol. It also
                     *         goes to the former event loop if the protocol
                     *         has finished.
                     */
                    class PreventBlockingOfMiddlewareCoreInBlockingProtocol {
                            volatile bool passed;
                            ::std::thread t;
                            ::std::mutex m;

                            void runMiddlewareCore();

                        public:
                            /*! \brief runs the middleware core in an additional
                             *         thread and synchronize it with further
                             *         instances of PreventBlocking...
                             */
                            PreventBlockingOfMiddlewareCoreInBlockingProtocol();

                            /*! \brief restores the former state by finishing
                             *         the thread and going back to the former
                             *         event processing loop
                             */
                            ~PreventBlockingOfMiddlewareCoreInBlockingProtocol();

                            /*! \brief process has to be called by the blocking
                             *         protocol to avoid busy waiting, allowing
                             *         further progress
                             */
                            void process();
                    };
                }
            } /* namespace CAN */
        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */

#endif

