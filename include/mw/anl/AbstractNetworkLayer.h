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

            /*! \brief The abstract network layer provides functionality that is not
             *         specific to a concrete network layer.
             *
             *          It has the ability to get different lower layer plugs and supports
             *          functionality like fragmentation, or in later versions, it handles
             *          also the aspects of quality of service or attribute management.
             *
             *  \tparam NL the network layer see prerequisites.
             *
             *  \pre    The type of template parameters can be an famouso::mw::nl::CANNL,
             *          famouso::mw::nl::AWDSNL, famouso::mw::nl::UDPMultiCastNL,
             *          famouso::mw::nl::UDPBroadCastNL or an famouso::mw::nl::VoidNL dependent
             *          on the configuration of the middleware stack
             */
            template < class NL >
            class AbstractNetworkLayer : public NL {
                public:

                    /*! \brief  short network representation of the subject
                     */
                    typedef typename NL::SNN SNN;

                    /*! \brief  short network representation of the subscribe subject
                     *          that is used for announcing subscribtion network-wide
                     */
                    SNN   subscribe_SNN;

                    /*! \brief Initalizes the sub networks and bind the subscription
                     *         management channel.
                     */
                    void init() {
                        NL::init();
                        Subject s("SUBSCRIBE");
                        NL::bind(s, subscribe_SNN);
                    }

                    /*! \brief  announce a subject and get its short network representation
                     *
                     *  \param[in]   s the subject that is announced
                     *  \param[out]  snn the short network name of the subject
                     */
                    void announce(const Subject &s, SNN &snn) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                        NL::bind(s, snn);
                        // nach dem Bind noch bekannt geben,
                        // das dieser Kanal publiziert wird
                    }

                    /*! \brief   publish an event on the pluged lower network
                     *
                     *  \param[in]  snn the short network name of the subject
                     *  \param[in]  e the event that has to be published
                     */
                    void publish(const SNN &snn, const Event &e) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                        typename NL::Packet_t p(snn, &e[0], e.length);
                        if (e.length <= NL::info::payload)
                            NL::deliver(p);
                        else
                            DEBUG(("Event is to big to deliver at once and fragmentation is not supported at the moment\n"));
                    }

                    /*! \brief  subscribe a subject and get its short network representation
                     *
                     *          The subscribtion contains out of two parts. First it binds
                     *          the subject to its short network name, and second it announces
                     *          the subscription to allow gateways the establishing of forwarding
                     *          channels.
                     *
                     *  \param[in]   s the subject that is announced
                     *  \param[out]  snn the short network name of the subject
                     */
                    void subscribe(const Subject &s, SNN &snn) {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                        NL::bind(s, snn);
                        // nach dem Bind auch noch bekannt geben,
                        // dass dieser Kanal subscribiert wird
                        typename NL::Packet_t p(subscribe_SNN, const_cast<uint8_t*>(s.tab()), 8);
                        NL::deliver(p);
                    }


                    /*! \brief  fetches an event from the network layer if the short network
                     *          name is equal to the short network name of the arosen packet.
                     *
                     *  \param[in]  snn the short network name of the subject
                     *  \param[out] e the event that has to be published
                     *  \param[in]  bnl the sub network in that the event \e e will be published.
                     *
                     *  \return \li \b true if \e snn and the snn of the last arosen packet are equal
                     *          \li \b false otherwise
                     */
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

                    /*! \brief Is called by the higher layer to signalise that
                     *         an event processing request was arised.
                     *
                     *  \param[in]  bnl the sub network-ID from where the request came.
                     */
                    void event_process_request(famouso::mw::nl::BaseNL * const bnl) {
                    }

                    /*! \brief Is called by the higher layer to signalise that
                     *         the event is processed now.
                     */
                    void event_processed() {
                    }

            };

        } // namespace anl
    } // namespace mw
} //namespace famouso

#endif /* __AbstractNetworkLayer_h__ */
