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

#ifndef __NETWORKGUARD_H_F8945680F71654__
#define __NETWORKGUARD_H_F8945680F71654__

namespace famouso {
    namespace mw {
        namespace guard {

            /*!
             *  \brief  Network guard
             *
             *  The network guard is an optional layer that can be placed between
             *  the AbstractNetworkLayer and the corresponding network layer. It is
             *  responsible for guaranteeing conformance to the transmission control
             *  protocol ensuring real time delivery.
             *
             *  \tparam NL  The network layer to use. See AbstractNetworkLayer for
             *              an overview of supported network layers.
             *  \tparam RTPolicy    This policy defines the protocol conformance test
             *                      that should be executed for real time packets.
             *                      If a real time packet fails the test, it is
             *                      omitted. Currently supported implementations are
             *                      RT_WindowCheck and RT_NoWindowCheck.
             *  \tparam NRTPolicy   The function of this policy is to ensure the
             *                      conformance of non real time packet delivery
             *                      to the protocol. Thus, delivery of NRT packets
             *                      is delayed until network access is granted.
             *                      Currently the implementations NRT_HandledByNL
             *                      and NRT_PollSlave are supported.
             */
            template <class NL,
                      template <class> class RTPolicy,
                      template <class> class NRTPolicy>
            class NetworkGuard : public NRTPolicy< RTPolicy< NL > > {

                    typedef NRTPolicy< RTPolicy< NL > >  Base;

                public:
                    /// Packet type defined by network layer
                    typedef typename NL::Packet_t Packet_t;

                    /// Policy invoked by AbstactNetworkLayer on event process request
                    typedef typename Base::EventProcessRequestPolicy EventProcessRequestPolicy;

                    /// Publish parameter set used as optional parameter for internal publish functions
                    typedef typename Base::PublishParamSet PublishParamSet;

                    /// Needed by ANL to check whether a network guard is used
                    typedef void NetworkGuardCheckTag;

                    /// Initialization of this and lower layers
                    void init() {
                        Base::init();
                    }

                    /*!
                     *  \brief  Deliver packet
                     *  \param  p   Packet to deliver
                     *
                     *  The behaviour depends on the RTPolicy and NRTPolicy
                     *  configuration. Real time packets are only delivered if
                     *  the RTPolicy grants access to the network. Access right
                     *  may be refused in case of timing errors. Delivery of
                     *  non real time packets is delayed until the NRTPolicy
                     *  grants access right to the network.
                     */
                    void deliver(const Packet_t & p) {
                        if (p.realtime) {
                            if (!Base::rt_access_right()) {
                                /*! \todo   In case the network guard refuses the right
                                 *          to deliver a real time packet (timing error),
                                 *          the publisher should be informed by calling
                                 *          the publisher exception callback.
                                 */
                                return;
                            }
                        } else {
                            // may block (and invoke the dispatcher to run another task)
                            Base::ensure_nrt_access_right();
                        }
                        NL::deliver(p);
                    }
            };

        } // namespace guard
    } // namespace mw
} //namespace famouso

#endif // __NETWORKGUARD_H_F8945680F71654__

