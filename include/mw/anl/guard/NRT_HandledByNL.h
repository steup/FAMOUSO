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

#ifndef __NRT_HANDLEDBYNL_H_E7017179B5C03D__
#define __NRT_HANDLEDBYNL_H_E7017179B5C03D__


namespace famouso {
    namespace mw {
        namespace guard {

            /*!
             *  \brief  NRT policy for network guard: empty version for configurations
             *          in which network layer handles non real time packet transmission
             *          control.
             *
             *  E.g. CANNL is able to exploit CAN priorities for transmission of NRT
             *  packets.
             */
            template <class NL>
            class NRT_HandledByNL : public NL {

                protected:
                    /// Returns when the NRT packet can be transmitted
                    void ensure_nrt_access_right() {
                    }

                public:
                    /// Initialization of this and lower layers
                    void init() {
                        NL::init();
                    }

                    /// Policy invoked by AbstactNetworkLayer on event process request
                    struct EventProcessRequestPolicy {
                        static bool process(NRT_HandledByNL & network_guard) {
                            return true;    // Continue event processing
                        }
                    };
            };

        } // namespace guard
    } // namespace mw
} //namespace famouso

#endif // __NRT_HANDLEDBYNL_H_E7017179B5C03D__

