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

#ifndef __ExtendedEvent_h__
#define __ExtendedEvent_h__

#include "mw/common/Event.h"
#include "mw/attributes/EmptyAttribute.h"

namespace famouso {
    namespace mw {
        /*!\brief definition of an extended event with attributes
         *
         * the binary representation is as follows
         \verbatim
           |C-AB|0..255 A|EP|
            C-AB -- number of attribute bytes
            A    -- the attributes in binary representation
            EP   -- event data/payload
         \endverbatim
         *
         * \tparam s is the size of the event payload
         * \tparam Attr is an attribute attached to the event
         *
         * \todo at the moment only one attribute is attachable. This
         *      should change to a list of attributes to be added.
         */
        template< uint16_t s=0,
                  typename Attr=::famouso::mw::attributes::EmptyAttribute
                >
        class ExtendedEvent : public ::famouso::mw::Event {
          public:
            enum {
                attr_length=Attr::size
            };

          private:
            /*!\brief the whole event with attributes and payload
             */
            uint8_t _edata[1+attr_length+s];

          public:
            ExtendedEvent (const famouso::mw::Subject &sub) : Event(sub) {
                _edata[0]=attr_length;
                new (&_edata[1]) Attr;
                length=s+1+attr_length;
                data=_edata;
            }

            /*!\brief payload setting as simple as possible
             */
            void operator = (const char* str) {
                uint16_t i=0;
                while( str[i] && (i<s)) {
                    _edata[1+attr_length+i]=str[i];
                    ++i;
                }
            }
        };

    } /* mw */
} /* famouso */

#endif
