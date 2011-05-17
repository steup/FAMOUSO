/*******************************************************************************
 *
 * Copyright (c) 2011 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
 * $Id: $
 *
 ******************************************************************************/

#define JOIN( X, Y ) DO_JOIN( X, Y )
#define DO_JOIN( X, Y ) DO_JOIN2(X,Y)
#define DO_JOIN2( X, Y ) X##Y

#include "config/type_traits/if_select_type.h"

namespace failed_assertion {

    struct args;

    template< bool C, typename T>
    int assertion_failed( typename if_select_type<(C), void*, void>::type, T);
}

#undef FAMOUSO_STATIC_ASSERT_ERROR
#define FAMOUSO_STATIC_ASSERT_ERROR(expr, msg, types)                       \
    struct JOIN(_error_msg_,msg);                                           \
    enum {                                                                  \
        JOIN(E,__LINE__)=                                                   \
            sizeof( ::failed_assertion::assertion_failed<(expr)>(           \
                static_cast<JOIN(_error_msg_,msg)*>(0),                     \
                static_cast< ::failed_assertion::args (*) types >(0)        \
            )                                                               \
        )                                                                   \
    }

template <typename t>
struct ClassTemplate {
    FAMOUSO_STATIC_ASSERT_ERROR(true, ClassTemplateContext, () );
    FAMOUSO_STATIC_ASSERT_ERROR(sizeof(t) == 0 && false, ClassTemplateContext, (t) );
};
ClassTemplate<int> lll;

template <typename t>
void FunctionTemplate () {
    FAMOUSO_STATIC_ASSERT_ERROR(true, FunctionTemplateContext, () );
    FAMOUSO_STATIC_ASSERT_ERROR(sizeof(t) == 0 && false, FunctionTemplateContext, (t,int) );
}

FAMOUSO_STATIC_ASSERT_ERROR(true, GlobalContext, () );
FAMOUSO_STATIC_ASSERT_ERROR(false, GlobalContext, () );


int main() {

    FAMOUSO_STATIC_ASSERT_ERROR(true, MainContext, () );
    FAMOUSO_STATIC_ASSERT_ERROR(false, MainContext, () );

    FunctionTemplate<long>();

}

