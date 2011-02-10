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

#ifndef __CHANNELTRAMPOLINE_H_ECE52897B24184__
#define __CHANNELTRAMPOLINE_H_ECE52897B24184__

#include "case/Delegate.h"
#include "mw/el/ml/ChannelReservationData.h"

namespace famouso {
    namespace mw {
        namespace api {
            namespace detail {

                /*!
                 *  \brief  Channel trampoline gives access to higher derivation levels
                 *          from EventChannel. It allows for querying information and
                 *          triggering actions there.
                 */
                struct ChannelTrampoline {

                    protected:

                        ChannelTrampoline() {
                            trampoline.bind<
                                    ChannelTrampoline,
                                    &ChannelTrampoline::dummy_trampoline_action_impl>(this);
                        }

                        struct Action {
                            enum {
                                /*! \brief Get the channel's requirement attributes
                                 *
                                 *  Action trigger function returns length of the
                                 *  attribute set and writes the binary
                                 *  representation into buffer if \e buffer does
                                 *  not equal NULL.
                                 */
                                get_requirements,

                                /*! \brief Set real time reservation state of a
                                 *         publisher concerning a specific network
                                 *
                                 *  The data is passed via \e buffer pointing to a
                                 *  ChannelReservationData struct.
                                 */
                                set_real_time_reserv_state,
                            } action;
                            uint8_t * buffer;
                        };

                        /*! \brief  Triggers a channel action that needs information
                         *          from higher class derivation level
                         *  \note   Only one delegate for multiple function (and no
                         *          virtuals) to save RAM.
                         *  \todo   Save RAM by replacing this with another type of delegate wich
                         *          does not store an object pointer, because the this pointer
                         *          is known at invocation time.
                         */
                        util::Delegate<Action &, uint16_t> trampoline;

                        uint16_t dummy_trampoline_action_impl(Action & action) {
                            if (action.action == Action::get_requirements) {
                                // Return empty requirement attribute set
                                typedef attributes::AttributeSet<> ReqAttr;
                                if (action.buffer) {
                                    new (action.buffer) ReqAttr;
                                }
                                return ReqAttr::overallSize;
                            }
                            return 0;
                        }

                    public:

                        /*! \brief get channel requirement attribute set (binary
                         *         representation)
                         *  \param buffer Buffer to store the binary
                         *                representation of the attribute set or
                         *                NULL.
                         *  \return length of the binary representation in bytes
                         *  \note   You must ensure that \p buffer is large
                         *          enough to hold the attribute set. Call this
                         *          function with a NULL argument to get the
                         *          size for buffer allocation.
                         */
                        uint16_t get_requirements(uint8_t * buffer) const {
                            Action a;
                            a.action = Action::get_requirements;
                            a.buffer = buffer;
                            return trampoline(a);
                        }


                        /*! \brief Sets the real time reservation state
                         *  \param crd  Contains real time reservation infos.
                         *  \note  This needs only to be implemented for real time
                         *         publishers.
                         */
                        void set_real_time_reserv_state(famouso::mw::el::ml::ChannelReservationData * crd) const {
                            Action a;
                            a.action = Action::set_real_time_reserv_state;
                            a.buffer = reinterpret_cast<uint8_t*>(crd);
                            trampoline(a);
                        }
                };

            } // namespace detail
        } // namespace api
    } // namespace mw
} // namespace famouso

#endif // __CHANNELTRAMPOLINE_H_ECE52897B24184__

