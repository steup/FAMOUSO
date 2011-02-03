/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __NEWEVENTLAYER_H_2A2AEDA368D391__
#define __NEWEVENTLAYER_H_2A2AEDA368D391__


template <class LL, template <class, class> class ManLayPolicy>
class NewEventLayer;

template <class LL, template <class, class> class ManLayPolicy>
class ELStackBuilder {
        typedef NewEventLayer<LL, ManLayPolicy> EL;
    public:
        typedef ManLayPolicy <
                    famouso::mw::el::EventDispatcher<
                        LL,
                        EL
                    >,
                    EL
                > type;
};

template <class LL, template <class, class> class ManLayPolicy>
class NewEventLayer : public ELStackBuilder<LL, ManLayPolicy>::type {

        /// First sublayer
        typedef typename ELStackBuilder<LL, ManLayPolicy>::type SL;

        typedef famouso::mw::api::EventChannel<NewEventLayer> EC;

    public:

        typedef typename SL::ChannelTrampolinePolicy ChannelTrampolinePolicy;

        void init() {
            SL::init();
        }

        void announce(EC & ec) {
            SL::announce(ec);
        }

        void unannounce(EC & ec) {
            SL::unannounce(ec);
        }

        void subscribe(EC & ec) {
            SL::subscribe(ec);
        }

        void unsubscribe(EC & ec) {
            SL::unsubscribe(ec);
        }

        void publish(const EC &ec, const Event &e) {
            SL::publish(ec, e);
        }

        void publish_local(const Event &e) {
            SL::publish_local(e);
        }

        void fetch(famouso::mw::nl::DistinctNL *bnl = 0) {
            // inform low layer about fetching starts
            LL::event_process_request(bnl);

            SL::fetch(bnl);

            // inform lower layer that we are done
            LL::event_processed();
        }
};


#endif // __NEWEVENTLAYER_H_2A2AEDA368D391__

