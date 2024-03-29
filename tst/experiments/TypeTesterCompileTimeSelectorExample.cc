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
#include <iostream>
#include "boost/mpl/bool.hpp"
#include "boost/mpl/assert.hpp"
#include "boost/static_assert.hpp"
#include "config/type_traits/if_contains_type.h"
#include "config/policies/StaticCreatorPolicy.h"
#include "config/policies/LocalCreatorPolicy.h"
#include "config/policies/ThenPolicyExample.h"
#include "config/policies/ElsePolicyExample.h"

struct Micha {
    Micha() {
        std::cout << __PRETTY_FUNCTION__  << std::endl;
    };
    ~Micha() {
        std::cout << __PRETTY_FUNCTION__  << std::endl;
    };
};
struct StructWithGW {
    typedef Micha gw;
};

struct StructWithNoGW {
    typedef int blubb;
};

struct ThenWithParamPolicy {
    template< typename T, typename SubType, typename R, typename P>
    static  R process(P p) {
        std::cout << "from then: " << p << std::endl;
        return true;
    }
};

struct ElseWithParamPolicy {
    template< typename T, typename SubType, typename R, typename P>
    static  R process(P p) {
        std::cout << "from else: " << p << std::endl;
        return false;
    }
};

IF_CONTAINS_TYPE_(gw);

int main() {
    std::cout << if_contains_type_gw<int>::value << std::endl;
    std::cout << if_contains_type_gw<StructWithGW>::value << std::endl;
    std::cout << if_contains_type_gw<StructWithNoGW>::value << std::endl;

    if_contains_type_gw<StructWithGW>::ThenElse<StaticCreatorPolicy>::process();
    if_contains_type_gw<StructWithGW>::ThenElse<LocalCreatorPolicy>::process();
    std::cout << if_contains_type_gw<StructWithGW, int>::ThenElse<ThenPolicyExample, ElsePolicyExample>::process()<< std::endl;
    std::cout << if_contains_type_gw<StructWithNoGW, int>::ThenElse<ThenPolicyExample, ElsePolicyExample>::process()<< std::endl;
    if_contains_type_gw<int>::ThenElse<ThenPolicyExample>::process();

    std::cout << if_contains_type_gw<int, bool, const char *>::ThenElse<
                        ThenWithParamPolicy,
                        ElseWithParamPolicy>::process("should be else")
              << std::endl;
    std::cout << if_contains_type_gw<StructWithGW, bool, const char *>::ThenElse<
                        ThenWithParamPolicy,
                        ElseWithParamPolicy>::process("should be then")
              << std::endl;

    return 0;
}
