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

#ifndef __KNOWNPUBSUBLIST_H_2C60BD21A8C5B3__
#define __KNOWNPUBSUBLIST_H_2C60BD21A8C5B3__

#include <list>
#include "case/Delegate.h"

#include "mw/common/NodeID.h"
#include "mw/el/ml/LocalChanID.h"

namespace famouso {
    namespace mw {
        namespace rt_net_sched {

            template <class PubSub>
            class KnownPubSubList : public std::list< std::pair<PubSub, bool> > {

                    typedef  std::list< std::pair<PubSub, bool> >  Base;

                public:

                    typedef typename Base::iterator iterator;

                    PubSub & get(iterator it) {
                        return it->first;
                    }

                    const NodeID & node_id(iterator it) const {
                        return it->first.node_id;
                    }

                    const el::ml::LocalChanID & lc_id(iterator it) const {
                        return it->first.lc_id;
                    }

                    bool & unannounced(iterator it) {
                        return it->second;
                    }

                    void begin_update(const NodeID & nid) {
                        // Mark all publisher of this node for unannouncement
                        iterator it = Base::begin();
                        while (it != Base::end()) {
                            if (node_id(it) == nid) {
                                unannounced(it) = true;
                            }
                            ++it;
                        }
                    }

                    // Update step: search publisher matching the node_id and lc_id, if found return 0 otherwise return new item for initialization
                    PubSub & update(const NodeID & nodeID, const el::ml::LocalChanID & lcID, bool & inserted) {
                        iterator it = Base::begin();
                        while (it != Base::end()) {
                            if (node_id(it) == nodeID && lc_id(it) == lcID) {
                                unannounced(it) = false;
                                inserted = false;
                                return get(it);
                            }
                            ++it;
                        }

                        Base::push_back(std::pair<PubSub, bool>(PubSub(), false));
                        inserted = true;
                        return Base::back().first;
                    }

                    // Return whether to remove
                    typedef util::Delegate<const PubSub &, bool> UnannounceCallback;

                    void end_update(const UnannounceCallback & unannounce_callback) {
                        iterator it = Base::begin();
                        while (it != Base::end()) {
                            if (unannounced(it)) {
                                if (unannounce_callback(get(it))) {
                                    it = Base::erase(it);
                                    continue;
                                }
                            }
                            ++it;
                        }
                    }

                    void log() {
                        iterator it = Base::begin();
                        while (it != Base::end()) {
                            get(it).log();
                            ++it;
                        }
                    }
            };


        } // namespace rt_net_sched
    } // namespace mw
} // namespace famouso

#endif // __KNOWNPUBSUBLIST_H_2C60BD21A8C5B3__

