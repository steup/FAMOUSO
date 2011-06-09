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

#ifndef __SubscriberEventChannel_h__
#define __SubscriberEventChannel_h__

#include "debug.h"
#include "mw/api/EventChannel.h"
#include "mw/api/CallBack.h"

namespace famouso {
    namespace mw {
        namespace api {

            template < class ECH >
            class SubscriberEventChannel : public EventChannel<ECH> {

                public:
                    /*! \brief This callback is called if an event occurs.
                     *
                     *  CallBack is a generic delegate whish allows for using
                     *  C-functions, normal as well as const class member
                     *  functions.
                     */
                    famouso::mw::api::SECCallBack callback;

                public:

                    SubscriberEventChannel(const Subject &s) : EventChannel<ECH>(s) {}

                    ~SubscriberEventChannel() {
                        unsubscribe();
                    }

                    /*! \brief subscribe the event channel and reserve resources
                     *         and call the needed functionalities within the
                     *         sublayers to bind the subject etc.
                     *
                     *         \sa EventLayer::subscribe
                     *         \sa AbstractNetworkLayer::subscribe
                     */
                    void subscribe() __attribute__ ((noinline)) {
                        TRACE_FUNCTION;
                        // initialize a dummy callback if necessary
                        if (!callback)
                            callback.bind<&famouso::mw::api::cb>();

                        EventChannel<ECH>::ech().subscribe(*this);
                    }

                protected:
                    void unsubscribe() __attribute__ ((noinline)) {
                        TRACE_FUNCTION;
                        EventChannel<ECH>::ech().unsubscribe(*this);
                    }

            };

        } // namespace api
    } // namespace mw
} // namespace famouso


#endif /* __SubscriberEventChannel_h__ */
