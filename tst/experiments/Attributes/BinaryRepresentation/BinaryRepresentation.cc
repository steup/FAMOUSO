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
 ******************************************************************************/

#include <stdint.h>

#include "boost/mpl/list.hpp"
#include "mw/attributes/Attribute.h"
#include "mw/attributes/filter/find.h"
#include "mw/attributes/AttributeSet.h"

#include "mw/common/ExtendedEvent.h"

#include "mw/attributes/access/Attribute_RT.h"

#include "logging/logging.h"

#include "AttribTests.h"

#include "typeinfo"

template <typename Attr, typename Event>
void testFind(Event& ev) {
    Attr* a = ev.template find<Attr>();

	if (a != NULL) {
		::logging::log::emit() << "Attribute with ID " << (uint16_t) Attr::id << " could be found" << ::logging::log::endl;
		::logging::log::emit() << "Value: " << ::logging::log::hex << (uint64_t) a->getValue() << ::logging::log::dec << ::logging::log::endl;
		::logging::log::emit() << "Size:  " << ::logging::log::hex << (uint64_t) a->length() << ::logging::log::dec << ::logging::log::endl;
	} else {
		::logging::log::emit() << "Attribute with ID " << (uint16_t) Attr::id << " could not be found" << ::logging::log::endl;
	}

	::logging::log::emit() << ::logging::log::endl;
}

template <typename Attr, typename Event>
void testSet(Event& ev, const typename Attr::value_type newValue) {
	printCase<Attr>();

	Attr* a = ev.template find<Attr>();

	if (a != NULL) {
		TO_HEX;
		DBG_MSG("Attribute: " << reinterpret_cast<size_t>(a));

		if (a->setValue(newValue)) {
			DBG_MSG((uint64_t) newValue << " set successfully, look: " << (uint64_t) a->getValue());
		} else {
			DBG_MSG("Setting value to " << (uint64_t) newValue << " failed!");
		}
	} else {
		DBG_MSG("Attribute with ID " << static_cast<uint16_t>(Attr::id) << " not found!");
	}
}

using namespace famouso::mw;
using namespace famouso::mw::attributes;

int main() {
	typedef boost::mpl::list<a8, a7, a6, a5, a4, a3, a2, a1>::type attribList;
	typedef ExtendedEvent<16, attribList> eventType;

	typedef AttributeSet<attribList> attrSet;

	TO_HEX;

	famouso::mw::Subject sub(0x01);

	eventType ev(sub);

	print(ev.data, sizeof(eventType));
	DBG_MSG("");

	testFind<a1, eventType>(ev);
	testFind<a2, eventType>(ev);
	testFind<a3, eventType>(ev);
	testFind<a4, eventType>(ev);
	testFind<a5, eventType>(ev);
	testFind<a6, eventType>(ev);
	testFind<a7, eventType>(ev);
	testFind<a8, eventType>(ev);

	testSet<a1, eventType>(ev, 1);

	testSet<a2, eventType>(ev, 2);

	testSet<a3, eventType>(ev, 0xFF);
	testSet<a3, eventType>(ev, 0xFFF); // Fails, 0xFFF does not fit, 0x3FF is maximum

	testSet<a4, eventType>(ev, 0xFEDC);
	testSet<a4, eventType>(ev, 0x1);

	testSet<a5, eventType>(ev, 0x1);

	testSet<a6, eventType>(ev, 0x1);

	testSet<a7, eventType>(ev, 0x1);

	testSet<a8, eventType>(ev, -0x1);

	attrSet* set = reinterpret_cast<attrSet*>(ev.data);

	const uint16_t contentLen = set->contentLength();

	print(ev.data, set->length());
	DBG_MSG("");

	DBG_MSG("SetCount:   " << set->size());
	DBG_MSG("ContentLen: " << contentLen);
	DBG_MSG("OverallLen: " << set->length());

	uint8_t buf[contentLen];

	set->content(buf);

	print(buf, contentLen);
}
