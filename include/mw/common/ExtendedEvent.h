/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Marcus Foerster <MarcusFoerster1@gmx.de>
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
 *****************************************************************************/

#ifndef _Extended_Event_
#define _Extended_Event_

#include <stdint.h>

#include "boost/mpl/aux_/na.hpp"

#include "object/PlacementNew.h"

#include "mw/common/Subject.h"
#include "mw/common/Event.h"

#include "mw/attributes/detail/AttributeSetProvider.h"
#include "mw/attributes/AttributeSet.h"

namespace famouso {
    namespace mw {

        /*!
         * \brief The extension of the Event class with support for %attributes.
         *
         *
         */
        template <famouso::mw::Event::Type payLoadSize = 0,
                  typename A1orSeq = boost::mpl::na, typename A2 = boost::mpl::na,
                  typename A3 = boost::mpl::na, typename A4 = boost::mpl::na,
                  typename A5 = boost::mpl::na, typename A6 = boost::mpl::na,
                  typename A7 = boost::mpl::na, typename A8 = boost::mpl::na,
                  typename A9 = boost::mpl::na, typename A10 = boost::mpl::na>
        class ExtendedEvent: public Event {
            private:
                typedef typename famouso::mw::attributes::detail::SetProvider<
                                                                   A1orSeq, A2, A3, A4,
                                                                   A5, A6, A7, A8, A9, A10
                                                                  >::attrSet attrSet;

            public:
                typedef ExtendedEvent type;

            private:
                static const famouso::mw::Event::Type attribsLen = attrSet::overallSize;

                // the whole event with attributes and payload
                uint8_t _edata[attribsLen + payLoadSize];

            public:
                ExtendedEvent(const famouso::mw::Subject& sub) :
                    Event(sub) {
                    // Construct the attributes
                    new (&_edata[0]) attrSet;

                    // Set the base class' members
                    length = attribsLen + payLoadSize;
                    data   = _edata;
                }

                // payload setting as simple as possible
                void operator =(const char* str) {
                    Type i = 0;

                    while (str[i] && (i < payLoadSize)) {
                        _edata[attribsLen + i] = str[i];
                        ++i;
                    }
                }

                template <typename Attr>
                Attr* find() {
                    return ((reinterpret_cast<attrSet*>(_edata))->find<Attr>());
                }

                template <typename Attr>
                const Attr* find() const {
                    return ((reinterpret_cast<attrSet*>(_edata))->find<Attr>());
                }
        };

    } // end namespace mw
} // end namespace famouso

#endif // _Extended_Event_
