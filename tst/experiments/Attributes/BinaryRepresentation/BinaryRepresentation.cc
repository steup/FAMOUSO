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

#include "mw/attributes/EmptyAttribute.h"

#include "Utils.h"
#include "AttribTests.h"

#include "AttribSequence.h"
#include "ExtendedEvent.h"

#include "logging/logging.h"

#include "Utils.h"

template <typename Attr, typename Event>
void testFind(const Event& ev) {
	Attr* a = ev.template find<Attr>();

	if (a != NULL) {
		::logging::log::emit() << "Attribute with ID " << (uint16_t) Attr::id << " could be found" << ::logging::log::endl;
		::logging::log::emit() << "Value: " << ::logging::log::hex << (uint64_t) a->get() << ::logging::log::dec << ::logging::log::endl;
	} else {
		::logging::log::emit() << "Attribute with ID " << (uint16_t) Attr::id << " could not be found" << ::logging::log::endl;
	}

	::logging::log::emit() << ::logging::log::endl;
}

template <typename T, T V>
void testBitCount() {
	const uint16_t bc = BitCount<T, V>::value;
	const uint16_t bt = ByteCount<T, V>::value;

	DBG_MSG(V << " - " << bc << " (" << bt << ')');
}

template <int8_t V>
void testBitCount_() {
	const uint16_t bc = BitCount<int8_t, V>::value;

	DBG_MSG(static_cast<int16_t>(V) << " - " << bc);
}
template <uint8_t V>
void testBitCount__() {
	const uint16_t bc = BitCount<uint8_t, V>::value;

	DBG_MSG(static_cast<uint16_t>(V) << " - " << bc);
}

int main() {

	/*
	testBitCount__<0x0>();
	testBitCount__<0x1>();
	testBitCount__<0xFF>();

	DBG_MSG("");

	testBitCount<uint16_t, 0x0>();
	testBitCount<uint16_t, 0xFF>();
	testBitCount<uint16_t, 0x1FF>();
	testBitCount<uint16_t, 0xFFFF>();

	DBG_MSG("");

	testBitCount<uint32_t, 0xFF>();
	testBitCount<uint32_t, 0x1FF>();
	testBitCount<uint32_t, 0x1FFFF>();
	testBitCount<uint32_t, 0x1FFFFFF>();

	DBG_MSG("");

	testBitCount<uint64_t, 0xFF>();
	testBitCount<uint64_t, 0x1FF>();
	testBitCount<uint64_t, 0x1FFFF>();
	testBitCount<uint64_t, 0x1FFFFFF>();
	testBitCount<uint64_t, 0x1FFFFFFFFull>();
	testBitCount<uint64_t, 0x1FFFFFFFFFFull>();

	DBG_MSG("");
	DBG_MSG("");

	testBitCount_<0x0>();
	testBitCount_<0x1>();
	testBitCount_<-0x1>();

	DBG_MSG("");

	testBitCount<int64_t, 0x0>();
	testBitCount<int64_t, -0x0>();
	testBitCount<int64_t, 0xFF>();
	testBitCount<int64_t, -0xFF>();
	testBitCount<int64_t, 0xFFFF>();
	testBitCount<int64_t, -0xFFFF>();
	testBitCount<int64_t, 0xFFFFFF>();
	testBitCount<int64_t, -0xFFFFFF>();
	*/

	a8 a;

	DBG_MSG(a.get());

	::logging::log::emit() << "Finished." << ::logging::log::endl;
}

/*
int main() {
	typedef boost::mpl::list<a1, a2, a3, a4, a5, a6, a7>::type attribList;
	typedef ExtendedEvent<16, attribList>                      eventType;

	AttribSequence<attribList> sequence;

	sequence.printRT();

	sequence.print();

	// print(sequence.data, sizeof(sequence.data));

	famouso::mw::Subject sub(0x01);

	eventType ev(sub);

	testFind<a1, eventType>(ev);
	testFind<a2, eventType>(ev);
	testFind<a7, eventType>(ev);

	//a3* a = NULL;
	//a = ev.find<a3>(a);

	// a->set(0x01);

	testFind<a3, eventType>(ev);

	testFind<Local, eventType>(ev);

	return (0);

	// Test cases:

	// I. System attributes

	// 1. Value fits unextended
	testAttribute<a1>("val unex");
	// 2. Value fits extended (but has less than 9 bits itself)
	testAttribute<a2>("val ex1");
	// 3. Value fits extended (but has at least 9 bits itself)
	testAttribute<a3>("val ex2");
	// 4. Length fits unextended
	testAttribute<a4>("len unex");
	// 5. Length fits extended
	testAttribute<a5>("len ex");

	// II. Non-system attributes

	// 1. Length fits unextended
	testAttribute<a6>();
	// 2. Length fits extended
	testAttribute<a7>();

}

*/
