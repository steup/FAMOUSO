/*******************************************************************************
 *
 * Copyright (c) 2011 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __RT_WINDOWCHECK_H_D183F88F73D157__
#define __RT_WINDOWCHECK_H_D183F88F73D157__

#include "RT_NoWindowCheck.h"
#include "timefw/Time.h"

namespace famouso {
    namespace mw {
        namespace guard {

            /*!
             *  \brief  Publish parameter set used with RT_WindowCheck policy
             */
            class WindowCheckPublishParamSet : public NoWindowCheckPublishParamSet {

                    typedef NoWindowCheckPublishParamSet Base;

                protected:
                    const timefw::Time * ready;
                    const timefw::Time * window;

                public:
                    /// Constructor for non real time event
                    WindowCheckPublishParamSet() {
                    }

                    /// Constructor for real time event
                    WindowCheckPublishParamSet(const timefw::Time & ready_time,
                                               const timefw::Time & window_duration) :
                                Base(ready_time, window_duration),
                                ready(&ready_time), window(&window_duration) {
                    }

                    /// Called at ANL to prepare lower layers for publishing with the supported parameters
                    template <class Type>
                    void process(Type & net_guard) const {
                        if (realtime()) {
                            net_guard.grant_rt_access(*ready, *window);
                        }
                    }
            };


            /*!
             *  \brief  RT policy for network guard: check for timing error
             *          before granting access
             *
             *  Only allows transmission of a real time packet if the clock is
             *  not out of sync and the current time is inside the transmission
             *  window.
             */
            template <class NRTPolicy>
            class RT_WindowCheck : public NRTPolicy {

                    /// Earliest time when transmission is allowed
                    timefw::Time ready;

                    /// Latest time when transmission is allowed
                    timefw::Time deadline;

                public:
                    /// Publish parameter set used as optional parameter for internal publish functions
                    typedef WindowCheckPublishParamSet PublishParamSet;

                    /// Initialization of this and lower layers
                    void init() {
                        NRTPolicy::init();
                    }

                    /// Returns whether a real time packet can be transmitted now
                    bool rt_access_right() {
                        if (timefw::TimeSource::out_of_sync()) {
                            ::logging::log::emit()
                                << "[NetGuard] Real time tx time error: clock out of sync\n";
                            return false;
                        }
                        timefw::Time curr = timefw::TimeSource::current();
                        if (/*curr < ready ||*/ deadline < curr) {
                            ::logging::log::emit()
                                << "[NetGuard] Real time tx time error: tx window "
                                << ready << " - " << deadline
                                << ", tx time check " << curr << '\n';
                            return false;
                        } else {
                            return true;
                        }
                    }

                    /*!
                     *  \brief  Grant network access for RT packet
                     *  \param  ready_time  Starting time of the window during which
                     *                      transmission is allowed
                     *  \param  window_duration Duration of the transmission window
                     *
                     *  \note   Called by WindowCheckPublishParamSet::process().
                     */
                    void grant_rt_access(timefw::Time ready_time,
                                         timefw::Time window_duration) {
                        ready = ready_time;
                        deadline = ready_time + window_duration;
                    }
            };

        } // namespace guard
    } // namespace mw
} //namespace famouso

#endif // __RT_WINDOWCHECK_H_D183F88F73D157__

