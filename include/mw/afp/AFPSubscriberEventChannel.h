/*******************************************************************************
 *
 * Copyright (c) 2009-2010 Philipp Werner <philipp.werner@st.ovgu.de>
 *                         Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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


#ifndef __AFPSUBSCRIBEREVENTCHANNEL_H_A97131CF417B97__
#define __AFPSUBSCRIBEREVENTCHANNEL_H_A97131CF417B97__


#include "mw/afp/Defragmentation.h"

#include "mw/common/Event.h"
#include "mw/common/Subject.h"
#include "mw/api/CallBack.h"


namespace famouso {
    namespace mw {
        namespace afp {


            using famouso::mw::Event;
            using famouso::mw::Subject;


            /*!
             * \brief Subscriber Event Channel with fragmentation support in the application layer
             * \tparam AFPDC AFP defragmentation config
             * \tparam SEC Subscriber Event Channel
             */
            template < class SEC, class AFPDC, class EventType = Event >
            class AFPSubscriberEventChannel {

                    /* The order of member declarations is important! The sec has to
                     * be destructed first to ensure that no callback will happen while
                     * dp is destructed. That's why dp has to be declared first.
                     */

                    /// AFP defragmentation processor
                    DefragmentationProcessor<AFPDC> dp;

                    /// Famouso subscriber event channel
                    SEC sec;

                    /*!
                     * \brief SubscriberEventChannel (sec) callback.
                     */
                    void incoming_event(famouso::mw::api::SECCallBackData& cbd) {
                        DefragmentationStep<AFPDC> ds(cbd.data, cbd.length);

                        dp.process_fragment(ds);

                        if (ds.event_complete()) {
                            EventType e(cbd.subject);
                            e.data = ds.get_event_data();
                            e.length = ds.get_event_length();

                            if (callback)
                                callback(e);

                            dp.event_processed(ds);
                        }
                    }


                public:
                    /*! \brief This callback is called if an event occurs.
                     *
                     *  It is a generic delegate which allows for using
                     *  C-functions, normal as well as const class member
                     *  functions.
                     */
                    famouso::util::Delegate<const EventType &> callback;

                    /*!
                     * \brief Constructor
                     * \param s Subject of the channel
                     * \param mtu MTU to use for defragmentation. Should be same for all publisher/subscriber of this subject.
                     */
                    AFPSubscriberEventChannel(const Subject &s, uint16_t mtu) :
                            dp(mtu), sec(s) {
                    }

                    /// Destructor
                    ~AFPSubscriberEventChannel() {
                    }

                    /*!
                     * \brief Subscribe this channel's subject.
                     */
                    void subscribe() {
                        sec.callback.template bind < typeof(*this), & AFPSubscriberEventChannel<SEC, AFPDC, EventType>::incoming_event > (this);
                        sec.subscribe();
                    }

                    /*!
                     * \brief Return this channel's subject
                     */
                    const Subject &subject() const {
                        return sec.subject();
                    }
            };


        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __AFPSUBSCRIBEREVENTCHANNEL_H_A97131CF417B97__

