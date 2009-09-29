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

#ifndef __PublisherEventChannel_h__
#define __PublisherEventChannel_h__

#include "mw/api/EventChannel.h"

namespace famouso {
    namespace mw {
        namespace api {

            template < class ECH >
            class PublisherEventChannel : public EventChannel<ECH> {

                public:

                    PublisherEventChannel(const Subject &s) : EventChannel<ECH>(s) {
                    }

                    ~PublisherEventChannel() {
                        unannounce();
                    }


                    /*! \brief announce the event channel and reserve resources
                     *         and call the needed functionalities within the
                     *         sublayers to bind the subject etc.
                     *
                     *         \sa EventLayer::announce
                     *         \sa AbstractNetworkLayer::announce
                     */
                    void announce() {
                        ::logging::log::emit< ::logging::Trace>() << FUNCTION_SIGNATURE << ::logging::log::endl;
                        EventChannel<ECH>::ech().announce(*this);
                    }


                    /*! \brief publish an event via the event channel
                     *
                     *         \sa EventLayer::publish
                     *         \sa AbstractNetworkLayer::publish
                     */
                    void publish(const Event& e) {
                        ::logging::log::emit< ::logging::Trace>() << FUNCTION_SIGNATURE << ::logging::log::endl;
                        EventChannel<ECH>::ech().publish(*this, e);
                    }

                private:
                    void unannounce() {
                        ::logging::log::emit< ::logging::Trace>() << FUNCTION_SIGNATURE << ::logging::log::endl;
                        EventChannel<ECH>::ech().unannounce(*this);
                    }

            };

        } // namespace api
    } // namespace mw
} // namespace famouso

#endif /* __PublisherEventChannel_h__ */
