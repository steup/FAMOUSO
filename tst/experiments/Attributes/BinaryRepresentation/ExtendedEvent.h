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
 * $Id:
 *
 *****************************************************************************/

#ifndef _ExtendedEvent_
#define _ExtendedEvent_

#include <stdint.h>

#include "boost/mpl/list.hpp"

#include "mw/common/Subject.h"
#include "mw/common/Event.h"

#include "AttribSequence.h"
#include "find_.h"

typedef boost::mpl::list<> emptyList;

template<famouso::mw::Event::Type payLoadSize = 0, typename AttrList = emptyList>
class ExtendedEvent : public famouso::mw::Event {
	private:
		typedef AttribSequence<AttrList> attrSeq;

	public:
		typedef ExtendedEvent type;

		enum {
			attribsLen = attrSeq::overallSize
		};

	private:
		// the whole event with attributes and payload
		uint8_t _edata[attribsLen + payLoadSize];

	public:
		ExtendedEvent(const famouso::mw::Subject &sub) : Event(sub) {
			// Construct the attributes
			new (&_edata[0]) attrSeq;

			length = attribsLen + payLoadSize;

			data = _edata;
		}

		// payload setting as simple as possible
		void operator = (const char* str) {
			Type i = 0;

			while(str[i] && (i < payLoadSize)) {
				_edata[attribsLen + i] = str[i];
				++i;
			}
		}

		template <typename Attr>
		Attr* find() const {
			// TODO If Attr not contained in the list above, return NULL

			return (find::find<Attr>(static_cast<const uint8_t*>(_edata)));
		}
};

#endif // _ExtendedEvent_
