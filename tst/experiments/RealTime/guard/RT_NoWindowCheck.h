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

#ifndef __RT_NOWINDOWCHECK_H_6CBC546E178875__
#define __RT_NOWINDOWCHECK_H_6CBC546E178875__

#include "mw/anl/detail/EmptyPublishParamSet.h"
#include "../TFW.h"

namespace famouso {
    namespace mw {
        namespace guard {

            /*!
             *  \brief  Publish parameter set used with RT_NoWindowCheck policy
             */
            class NoWindowCheckPublishParamSet : public anl::detail::EmptyPublishParamSet {
                    typedef EmptyPublishParamSet Base;

                protected:
                    bool rt;

                public:
                    /// Constructor for non real time event
                    NoWindowCheckPublishParamSet() : rt(false) {
                    }

                    /// Constructor for real time event
                    NoWindowCheckPublishParamSet(const time::Time & ready_time,
                                                 const time::Time & window_duration) :
                                rt(true) {
                    }

                    /// Returns whether we want to publish a real time event
                    bool realtime() const {
                        return rt;
                    }

                    /// Called at ANL to prepare lower layers for publishing with the supported parameters
                    template <class Type>
                    void process(Type & net_guard) const {
                    }
            };


            /*!
             *  \brief  RT policy for network guard: always grant access
             *
             *  Assumes that no timing errors occur and saves ressources
             *  for error checking.
             */
            template <class NRTPolicy>
            class RT_NoWindowCheck : public NRTPolicy {

                public:
                    /// Publish parameter set used as optional parameter for internal publish functions
                    typedef NoWindowCheckPublishParamSet PublishParamSet;

                    /// Initialization of this and lower layers
                    void init() {
                        NRTPolicy::init();
                    }

                    /// Returns whether a real time packet can be transmitted now
                    bool rt_access_right() {
                        return true;
                    }

                    void grant_rt_access(time::Time ready_time,
                                         time::Time window_duration) {
                    }
            };

        } // namespace guard
    } // namespace mw
} //namespace famouso

#endif // __RT_NOWINDOWCHECK_H_6CBC546E178875__

