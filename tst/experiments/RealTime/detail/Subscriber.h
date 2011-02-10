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

#ifndef __SUBSCRIBER_H_40DF327C2B24CC__
#define __SUBSCRIBER_H_40DF327C2B24CC__

#include <stdint.h>

#include "mw/common/Subject.h"
#include "mw/common/NodeID.h"
#include "mw/el/ml/LocalChanID.h"
#include "mw/el/ml/NetworkID.h"


namespace famouso {
    namespace mw {
        namespace rt_net_sched {

            struct Subscriber {
                NodeID node_id;

                /// Local channel ID
                el::ml::LocalChanID lc_id;

                el::ml::NetworkID network_id;

                Subject subject;

                void init(const NodeID & node_id,
                          const el::ml::LocalChanID & lc_id,
                          const el::ml::NetworkID & network_id,
                          const Subject & subj) {
                    this->node_id = node_id;
                    this->lc_id = lc_id;
                    this->network_id = network_id;
                    this->subject = subj;
                }

                void log() {
                    using namespace ::logging;
                    log::emit()
                        << "- Subscriber " << log::hex << lc_id << ": subject " << subject
                        << ", node_id " << node_id
                        << ", network_id " << network_id
                        << ::logging::log::endl;
                }
            };

        } // namespace rt_net_sched
    } // namespace mw
} // namespace famouso

#endif // __SUBSCRIBER_H_40DF327C2B24CC__

