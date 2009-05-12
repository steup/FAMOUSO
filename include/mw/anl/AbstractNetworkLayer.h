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

#ifndef __AbstractNetworkLayer_h__
#define __AbstractNetworkLayer_h__

#include "debug.h"
#include "mw/common/Event.h"

namespace famouso {
    namespace mw {
        namespace anl {

            template < class NL >
            class AbstractNetworkLayer : public NL {
                public:

                    typedef typename NL::SNN SNN;
                    SNN   subscribe_SNN;

                    void announce(const Subject &s, SNN &snn) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                        NL::bind(s, snn);
                        // nach dem Bind noch bekannt geben,
                        // das dieser Kanal publiziert wird
                    }

                    void publish(const SNN &snn, const Event &e) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                        typename NL::Packet_t p(snn, &e[0], e.length);
                        if (e.length <= NL::info::payload)
                            NL::deliver(p);
                        else
                            DEBUG(("Event is to big to deliver at once and fragmentation is not supported at the moment\n"));
                    }

                    void subscribe(const Subject &s, SNN &snn) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                        NL::bind(s, snn);
                        // nach dem Bind auch noch bekannt geben,
                        // dass dieser Kanal subscribiert wird
                        typename NL::Packet_t p(subscribe_SNN, const_cast<uint8_t*>(s.tab()), 8);
                        NL::deliver(p);
                    }

                    bool fetch(const SNN &snn, Event &e, const famouso::mw::nl::BaseNL *bnl) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                        if (snn == NL::lastPacketSNN()) {
                            typename NL::Packet_t p;
                            NL::fetch(p);
                            e.length = p.data_length;
                            e.data = p.data;
                            return true;
                        } else {
                            return false;
                        }
                    }

                    // vorgesehen, um anzuzeigen, dass das Event fetch request
                    // vorliegt
                    void event_process_request(const famouso::mw::nl::BaseNL * const bnl) const {}

                    // vorgesehen, um anzuzeigen, dass das Event vollstaendig
                    // propagiert wurde und die moeglicherweise reservierten
                    // Ressouren wiederverwendet werden koennen
                    void event_processed() const {}


                    void init() {
                        NL::init();
                        Subject s("SUBSCRIBE");
                        NL::bind(s, subscribe_SNN);
                    }

            };

        } // namespace anl
    } // namespace mw
} //namespace famouso

#endif /* __AbstractNetworkLayer_h__ */
