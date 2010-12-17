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

#ifndef __EventChannel_h__
#define __EventChannel_h__

#include "mw/common/Subject.h"
#include "mw/common/Event.h"
#include "mw/el/EventChannelHandler.h"
#include "mw/attributes/AttributeSet.h"

#include "object/Chain.h"
#include "case/Delegate.h"

namespace famouso {
    namespace mw {
        namespace api {

            /*! \file EventChannel.h
             *
             * \class EventChannel
             *
             * \brief This is the base class of each EventChannel
             *
             *  The EventChannel encapsulates the needed data structures to handle the
             *  publish/subscribe communication and implements the redirection of the calls
             *  to the EventLayer (famouso::mw::el) also.
             *
             * \sa famouso::mw::el::EventLayer
             */
            template < class ECH >
            class EventChannel : public Chain {
                    // the 64Bit subject
                    Subject _subj;
                    // definition of the short network names of a subjectes
                    typename ECH::SNN _snn;

                protected:
                    struct MWAction {
                        enum {
                            /*! \brief Get the channel's requirement attributes
                             *
                             *  Action trigger function returns length of the
                             *  attribute set and writes the binary
                             *  representation into buffer if \e buffer does
                             *  not equal NULL.
                             */
                            get_requirements,

                            /*! \brief Start to publish events to network on
                             *         a real time channel
                             *
                             *  The data is passed via \e buffer pointing to a
                             *  ChannelReservationData struct.
                             */
                            start_real_time_delivery
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
                     *  \todo   Save RAM for configurations without a management channel by
                     *          leaving it out in that cases.
                     */
                    util::Delegate<MWAction &, uint16_t> mw_action_trampoline;

                    uint16_t dummy_mw_action_impl(MWAction & mw_action) {
                        if (mw_action.action == MWAction::get_requirements) {
                            // Return empty requirement attribute set
                            typedef attributes::AttributeSet<> ReqAttr;
                            if (mw_action.buffer) {
                                new (mw_action.buffer) ReqAttr;
                            }
                            return ReqAttr::overallSize;
                        }
                        return 0;
                    }

                    template <class LL, class EL> friend class ManagementLayer;

                public:
                    typedef ECH eventChannelHandler;

                    /*! \brief get the local event channel handler object
                     */
                    ECH& ech() const {
                        return famouso::mw::el::EventChannelHandler<ECH>::ech();
                    }

                    /*! \brief give the Short Network Name representation of
                     *         the subject corresponding to that event channel
                     */
                    typename ECH::SNN& snn() {
                        return _snn;
                    }

                    /*! \brief give the Short Network Name representation of
                     *         the subject corresponding to that event channel
                     *         in case of a const object instance
                     */
                    const typename ECH::SNN& snn() const {
                        return _snn;
                    }

                    /*! \brief give the 64Bit subject of that event channel
                     */
                    const Subject &subject() const {
                        return _subj;
                    }

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
                        MWAction mwa;
                        mwa.action = MWAction::get_requirements;
                        mwa.buffer = buffer;
                        return mw_action_trampoline(mwa);
                    }

                protected:
                    /*! \brief Constructor of the class is protected to avoid
                     *         instanciating objects.
                     */
                    EventChannel(const Subject& s) : _subj(s) {
                        mw_action_trampoline.template bind<
                                EventChannel,
                                &EventChannel::dummy_mw_action_impl>(this);
                    }


                private:
                    // Private copy constructor and copy ensure that event channels
                    // cannot be copied
                    EventChannel(const EventChannel<ECH> & e);
                    const EventChannel<ECH> & operator=(const EventChannel<ECH> &);
            };

        } // namespace api
    } // namespace mw
} // namespace famouso


#endif /* __EventChannel_h__ */
