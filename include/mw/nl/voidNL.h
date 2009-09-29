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

#ifndef __voidNL_h__
#define __voidNL_h__

#include "mw/nl/BaseNL.h"
#include "mw/nl/Packet.h"
#include "mw/common/NodeID.h"
#include "mw/common/Subject.h"
#include "mw/api/EventChannel.h"
#include "debug.h"
#include <stdio.h>

namespace famouso {
    namespace mw {
        namespace nl {

            class voidNL : public BaseNL {
                public:
                    struct info {
                        enum {
                            payload = 8
                        };
                    };

                    typedef uint16_t SNN;

                    typedef Packet<SNN> Packet_t;


                    voidNL() {}
                    ~voidNL() {}

                    void init() {
                        init(UID());
                    }
                    void init(const NodeID &i) {
                        ::logging::log::emit< ::logging::Trace>() << FUNCTION_SIGNATURE << ::logging::log::endl;
                        ::logging::log::emit< ::logging::Trace>()
                            << " Configuration 64Bit NodeID=" << i.value()
                            << ::logging::log::endl;
                    }

                    // bind Subject to specific networks name
                    void bind(const Subject &s, SNN &snn) {
                        ::logging::log::emit< ::logging::Trace>() << FUNCTION_SIGNATURE << ::logging::log::endl;
                        snn = 0x1;
                    }

                    void deliver(const Packet_t& p) {
                        ::logging::log::emit< ::logging::Trace>() << FUNCTION_SIGNATURE << ::logging::log::endl;
                    }

                    void deliver_fragment(const Packet_t& p) {
                        ::logging::log::emit< ::logging::Trace>() << FUNCTION_SIGNATURE << ::logging::log::endl;
                    }

                    void fetch(Packet_t& p) {
                        ::logging::log::emit< ::logging::Trace>() << FUNCTION_SIGNATURE << ::logging::log::endl;
                    }

                    void interrupt() {
                        ::logging::log::emit< ::logging::Trace>() << FUNCTION_SIGNATURE << ::logging::log::endl;
                    }

                    SNN lastPacketSNN() {
                        return 0xff;
                    }
            };
        } // namespace nl
    } // namespace mw
} //namespace famouso

#endif /* __voidNL_h__ */

