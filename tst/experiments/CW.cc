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

#include "boost/mpl/eval_if.hpp"
#include <stdint.h>

template<int>
struct CompileTimeWarningTest;

struct CompileTimeAssertion{
    template<typename>
    struct warning {
        typedef warning type;
        enum _ {w};
    };
};
struct CompileTime{
//    typedef CompileTime type
    template<typename>
    struct warning_in_line_{
        typedef warning_in_line_ type;
        enum _ {w};
    };
};

struct Integer {
    typedef Integer type;
    static const unsigned int w=8;
};

template<int i>
struct Ret {
    uint8_t array[i];
};

Ret<1> CountParameter(void (*)()){ return Ret<1>();}
template<typename T1>
Ret<2> CountParameter(void (*)(T1)) { return Ret<2>();}
template<typename T1, typename T2>
Ret<3> CountParameter(void (*)(T1,T2)) { return Ret<3>();}
template<typename T1, typename T2, typename T3>
Ret<4> CountParameter(void (*)(T1,T2,T3)) { return Ret<4>();}

namespace failed_assertion {

struct args;
struct arguments_define_as;
template<int>
struct argument_count;

    template<typename,typename>
    struct _warning_ {
        typedef _warning_ type;
        enum _ {w};
    };
}

template<int>
struct with {
    struct assertion_arguments{};
};

#define MSG failed_assertion_in_line_
#define MSG2 _with_message_
#define PRE __
#define POST __
// Solution which works in all contexts
#define FAMOUSO_STATIC_ASSERT_WARNING(expr, msg, types)                     \
            struct JOIN(JOIN(MSG,__LINE__),MSG2) {                          \
                enum JOIN(JOIN(PRE,msg),POST) {w};                          \
                static void args types { }                                  \
            };                                                              \
            typedef CompileTimeWarningTest<                                 \
                        JOIN(JOIN(MSG,__LINE__),MSG2)::w                    \
                            ==                                              \
                        boost::mpl::eval_if_c<                              \
                            expr,                                           \
                            Integer,                                        \
                            failed_assertion::_warning_<                       \
                                failed_assertion::argument_count<(sizeof(CountParameter(JOIN(JOIN(MSG,__LINE__),MSG2)::args))-1)>,          \
                               void**** (failed_assertion::args::****) types >                 \
                        >::type::w                                          \
                    > JOIN(__warning_in_line__,__LINE__)

template <typename t>
struct ClassTemplate {
    FAMOUSO_STATIC_ASSERT_WARNING(true, ClassTemplateContext, () );
    FAMOUSO_STATIC_ASSERT_WARNING(sizeof(t) == 0 && false, ClassTemplateContext, (t) );
};
ClassTemplate<int> lll;

template <typename t>
void FunctionTemplate () {
    FAMOUSO_STATIC_ASSERT_WARNING(true, FunctionTemplateContext, () );
    FAMOUSO_STATIC_ASSERT_WARNING(sizeof(t) == 0 && false, FunctionTemplateContext, (t,int) );
}

FAMOUSO_STATIC_ASSERT_WARNING(true, GlobalContext, () );
FAMOUSO_STATIC_ASSERT_WARNING(false, GlobalContext, () );

int main() {

    FAMOUSO_STATIC_ASSERT_WARNING(true, MainContext, () );
    FAMOUSO_STATIC_ASSERT_WARNING(false, MainContext, () );

    FunctionTemplate<long>();
}

