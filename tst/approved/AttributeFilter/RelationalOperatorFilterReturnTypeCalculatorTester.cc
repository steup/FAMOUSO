/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#include "mw/attributes/filter/exists.h"

#include "mw/attributes/tags/AttributeTag.h"
#include "mw/attributes/tags/SameTag.h"
#include "mw/attributes/tags/IntegralConstTag.h"
#include "mw/attributes/TTL.h"
#include "mw/attributes/Attribute.h"
#include "mw/attributes/detail/tags/TagSet.h"
#include "mw/attributes/detail/tags/HasLessThanRelation.h"
#include "mw/attributes/filter/RelationalOperatorFilterReturnTypeCalculator.h"

using namespace famouso::mw::attributes;
using namespace famouso::mw::attributes::filter;

typedef detail::TagSet<detail::HasLessThanRelation> tagSet;

// defines a configurable user defined attribute
template<uint8_t v>
class UserDefined : public Attribute<UserDefined<0>, tags::same_tag, uint8_t, v, 42, tagSet> {
public:
    typedef UserDefined type;
};

#include "mw/attributes/IntegralConstToType.h"
typedef IntegralConstToType<1> I2T;

struct TTT {};

#include "iostream"

int main(int argc, char **argv) {
{
    // integral type is allowed on time-to-life attribute
    typedef RelationalOperatorFilterReturnTypeCalculator < TTL<0>, int >::type                          t1;
    // same type is always allowed
    typedef RelationalOperatorFilterReturnTypeCalculator < TTL<0>, TTL<1> >::type                       t2;
    // integral constant is allowed on time-to-life attribute
    typedef RelationalOperatorFilterReturnTypeCalculator < TTL<0>, I2T >::type                          t3;
    // other attribute is not allowed
//    typedef RelationalOperatorFilterReturnTypeCalculator < TTL<0>, UserDefined<1> >::type               t4;
    // wrong type is never allowed with the attribute grammar
//    typedef RelationalOperatorFilterReturnTypeCalculator < TTL<0>, TTT >::type                          t5;
}
{
    // integral type is not allowed on time-to-life attribute
//    typedef RelationalOperatorFilterReturnTypeCalculator < UserDefined<0>, int >::type                  t1;
    // same type is always allowed
    typedef RelationalOperatorFilterReturnTypeCalculator < UserDefined<0>, UserDefined<1> >::type       t2;
    // integral constant is allowed on time-to-life attribute
//    typedef RelationalOperatorFilterReturnTypeCalculator < UserDefined<0>, I2T >::type                  t3;
    // other attribute is not allowed
//    typedef RelationalOperatorFilterReturnTypeCalculator < UserDefined<0>, TTL<0> >::type               t4;
    // wrong type is never allowed with the attribute grammar
//    typedef RelationalOperatorFilterReturnTypeCalculator < UserDefined<0>, TTT >::type                  t5;
}
    return 0;
}
