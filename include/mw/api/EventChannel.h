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

#ifndef __EventChannel_h__
#define __EventChannel_h__

#include "mw/common/Subject.h"
#include "mw/common/Event.h"
#include "mw/common/AttributeList.h"
#include "mw/common/CallBack.h"

#include "debug.h"
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
                    // definition of the event channel handler
                    static uint8_t _ech[sizeof(ECH)];
                    // the 64Bit subject
                    Subject _subj;
                    // definition of the short network names of a subjectes
                    typename ECH::SNN _snn;

                public:
                    /*! \brief get the local event channel handler object
                     */
                    static inline ECH& ech() {
                        return reinterpret_cast<ECH&>(_ech);
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


                protected:
                    /*! \brief Constructor of the class is protected to avoid
                     *         instanciating objects.
                     */
                    EventChannel(const Subject& s) : _subj(s) {
                        DEBUG(("%s ECH-ADDR=%p\n", __PRETTY_FUNCTION__, &_ech));
                    }

            };

            /*! \brief The local event channel handler object.
             *
             * \todo open question, whether the ECH should be static or accessible via a singleton ???
             */
            template < class ECH > uint8_t EventChannel<ECH>::_ech[sizeof(ECH)] = {0};

        } // namespace api
    } // namespace mw
} // namespace famouso


#endif /* __EventChannel_h__ */
