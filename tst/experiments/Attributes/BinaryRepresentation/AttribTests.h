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

#ifndef _AttribTests_
#define _AttribTests_

#include <stdint.h>

#include "boost/mpl/int.hpp"

#include "logging/logging.h"

#include "mw/attributes/Attribute.h"
#include "mw/attributes/AttributeSet.h"

#include "mw/attributes/detail/AttributeHeader.h"
#include "mw/attributes/tags/IntegralConstTag.h"

#define TO_HEX ::logging::log::emit() << ::logging::log::hex
#define TO_DEC ::logging::log::emit() << ::logging::log::dec
#define DBG_MSG(x) ::logging::log::emit() << x << ::logging::log::endl

typedef famouso::mw::attributes::tags::integral_const_tag tag;

void print(const uint8_t* array, uint8_t length) {
    ::logging::log::emit() << ::logging::log::hex;

    for (uint8_t i = 0; i < length; ++i) {
        ::logging::log::emit() << '{' << reinterpret_cast<size_t> (&array[i])
                << "}=";
        ::logging::log::emit() << '[' << static_cast<uint16_t> (array[i])
                << ']';
    }

    ::logging::log::emit() << ::logging::log::endl;

    // ::logging::log::emit() << ::logging::log::dec;
}

template <typename Attr>
void printCase() {
    ::logging::log::emit() << "Selected case "
            << static_cast<uint16_t> (famouso::mw::attributes::detail::CaseSelector<Attr, uint16_t, 1, 2, 3, 4, 5, 6>::value)
            << ::logging::log::endl;
}

template <typename AttrType>
void testAttribute(const char* text = "") {
    ::logging::log::emit() << text << ::logging::log::endl;
    ::logging::log::emit() << ::logging::log::hex;
    ::logging::log::emit() << "Testing attribute with ID "
            << (int) AttrType::id << " and value " << AttrType::value
            << ::logging::log::endl;
    ::logging::log::emit() << ::logging::log::dec;
    ::logging::log::emit() << ::logging::log::endl;

    uint8_t data[famouso::mw::attributes::detail::AttributeSize<AttrType>::value];

    ::logging::log::emit() << "Value offset: "
            << famouso::mw::attributes::detail::ValueOffset<AttrType>::value
            << ::logging::log::endl;
    ::logging::log::emit() << ::logging::log::endl;

    // Construct the attribute into the data array at the correct offset
    new (&data[famouso::mw::attributes::detail::ValueOffset<AttrType>::value]) AttrType;

    ::logging::log::emit() << "Attribute size     : "
            << famouso::mw::attributes::detail::AttributeSize<AttrType>::value
            << ::logging::log::endl;
    ::logging::log::emit() << "Attribute bit count: "
            << famouso::mw::attributes::detail::ValueBitCount<AttrType>::value
            << ::logging::log::endl;
    ::logging::log::emit() << ::logging::log::endl;

    // Now construct the header into the array
    new (&data[0]) famouso::mw::attributes::detail::AttributeHeader<AttrType>;

    ::logging::log::emit() << "Header size: "
            << (int) famouso::mw::attributes::detail::AttributeHeader<AttrType>::size
            << ::logging::log::endl;
    ::logging::log::emit() << ::logging::log::endl;

    print(data, sizeof(data));

    ::logging::log::emit() << "------------------------------------------"
            << ::logging::log::endl;
    ::logging::log::emit() << ::logging::log::endl;
}

template <uint8_t ttl>
struct TTL: public famouso::mw::attributes::Attribute<TTL<0>, tag,
                                                      uint8_t, ttl, 0, true> {
    typedef TTL type;
};

template <uint16_t delay>
struct Delay: public famouso::mw::attributes::Attribute<Delay<0>, tag,
                                                        uint16_t, delay, 1, true> {
    typedef Delay type;
};

template <uint16_t omission>
struct OmissionDegree: public famouso::mw::attributes::Attribute<OmissionDegree<0>,
                                                                 tag, uint16_t,
                                                                 omission, 2, true> {
    typedef OmissionDegree type;
};

typedef TTL<0> Local;
typedef TTL<1> Body;
typedef TTL<255> World;

typedef OmissionDegree<0> None;

template <typename VT, VT V, uint8_t ID, bool IS>
struct TestAttrib : public famouso::mw::attributes::Attribute<boost::mpl::int_<ID>, tag, VT, V, ID, IS> {
    typedef TestAttrib type;
};

typedef TestAttrib<uint8_t,  0x3,                  1, true>::type  a1;
typedef TestAttrib<uint8_t,  0x9,                  2, true>::type  a2;
typedef TestAttrib<uint16_t, 0x3FF,                3, true>::type  a3;
typedef TestAttrib<uint16_t, 0x4FF,                4, true>::type  a4;
typedef TestAttrib<uint64_t, 0x1FFFFFF,            5, true>::type  a5;
typedef TestAttrib<uint8_t,  2,                    6, false>::type a6;
typedef TestAttrib<uint64_t, 0x1FFFFFFFFFFFFFFull, 7, false>::type a7;
typedef TestAttrib<int16_t, -20,                   8, false>::type a8;

typedef TestAttrib<int16_t, -20,                   9, false>::type a9;

#endif
