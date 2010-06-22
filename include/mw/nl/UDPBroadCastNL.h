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

#ifndef __UDPBroadCastNL_h__
#define __UDPBroadCastNL_h__

#include "util/ios.h"
#include "mw/nl/DistinctNL.h"
#include "mw/nl/Packet.h"
#include "mw/afp/Config.h"
#include "mw/common/NodeID.h"
#include "mw/common/Subject.h"
#include "mw/api/EventChannel.h"
#include "mw/el/EventLayerCallBack.h"
#include "debug.h"
#include <stdio.h>

namespace famouso {
    namespace mw {
        namespace nl {

            class UDPBroadCastNL : public DistinctNL {
                public:
                    struct info {
                        enum {
                            mtu = 8192,
                            payload = mtu - sizeof(famouso::mw::Subject) - sizeof(bool),
                        };
                    };

                    typedef famouso::mw::Subject SNN;

                    typedef Packet<SNN> Packet_t;

                    /// Default AFP %configuration of this network layer (see \ref afp_config_nl)
                    typedef afp::Disable AFP_Config;

                    UDPBroadCastNL() { }
                    ~UDPBroadCastNL() {}

                    void init(const NodeID &i) {
                        TRACE_FUNCTION;
                        ::logging::log::emit< ::logging::Info>()
                            << PROGMEMSTRING(" Configuration 64Bit NodeID=")
                            << i.value()
                            << ::logging::log::endl;
                    }

                    // bind Subject to specific networks name
                    void bind(const Subject &s, SNN &snn) {
                        TRACE_FUNCTION;
                    }

                    void deliver(const Packet_t& p) {
                        TRACE_FUNCTION;
                    }

                    void fetch(Packet_t& p) {
                        TRACE_FUNCTION;
                    }

                    void interrupt() {
                        TRACE_FUNCTION;
                    }

                    void init() {
                        TRACE_FUNCTION;
                        famouso::util::impl::start_ios();
                    }
                    SNN lastPacketSNN() {
                        return 0;
                    }
            };
        }
    }
}
#endif /* __UDPBroadCastNL_h__ */

