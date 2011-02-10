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

#ifndef __RTNETSCHEDULERBASE_H_DA957A38AD3639__
#define __RTNETSCHEDULERBASE_H_DA957A38AD3639__

namespace famouso {
    namespace mw {
        namespace rt_net_sched {

            /*!
             *  \brief  Real time network scheduler base class
             *
             *  Used as a publishing interface for NetworkScheduler.
             */
            class RTNetSchedulerBase {
                public:
                    virtual void publish_reservation(const NodeID & node_id,
                                                     const el::ml::LocalChanID & lc_id,
                                                     const el::ml::NetworkID & net_id,
                                                     uint64_t tx_ready_time,
                                                     uint64_t tx_window_time) = 0;
                    virtual void publish_no_reservation(const NodeID & node_id,
                                                        const el::ml::LocalChanID & lc_id,
                                                        const el::ml::NetworkID & net_id) = 0;
                    virtual void publish_no_subscriber(const NodeID & node_id,
                                                       const el::ml::LocalChanID & lc_id,
                                                       const el::ml::NetworkID & net_id) = 0;
            };

        } // namespace rt_net_sched
    } // namespace mw
} // namespace famouso

#endif // __RTNETSCHEDULERBASE_H_DA957A38AD3639__

