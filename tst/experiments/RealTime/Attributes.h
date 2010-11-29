/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __ATTRIBUTES_H_9A5C730A2145CB__
#define __ATTRIBUTES_H_9A5C730A2145CB__

#include <stdint.h>

#include "mw/attributes/Attribute.h"
#include "mw/attributes/tags/IntegralConstTag.h"
#include "mw/attributes/filter/less_than_or_equal_to.h"
#include "mw/attributes/filter/equal_to.h"

#include "mw/attributes/detail/tags/TagSet.h"
#include "mw/attributes/detail/tags/HasLessThanRelation.h"

#include "mw/attributes/Period.h"

struct AttributeIDs {
    enum {
        maxEventSizeId = 16,
        realTimeId = 17
    };
};

template <uint16_t size>
class MaxEventSize : public famouso::mw::attributes::Attribute<
                                MaxEventSize<0>,
                                famouso::mw::attributes::tags::integral_const_tag,
                                uint16_t,
                                size,
                                AttributeIDs::maxEventSizeId,
                                famouso::mw::attributes::detail::TagSet<famouso::mw::attributes::detail::HasLessThanRelation> // less than or equal to!
                            > {
    public:
        typedef MaxEventSize type;
};

// Value: RT state (inital highest) TODO
class RealTime : public famouso::mw::attributes::Attribute<
                                RealTime,
                                famouso::mw::attributes::tags::integral_const_tag,
                                uint8_t,
                                1,
                                AttributeIDs::maxEventSizeId,
                                famouso::mw::attributes::detail::TagSet<famouso::mw::attributes::detail::HasLessThanRelation> // TODO: equal_to filter!!
                            > {
    public:
        typedef RealTime type;
};

#endif // __ATTRIBUTES_H_9A5C730A2145CB__

