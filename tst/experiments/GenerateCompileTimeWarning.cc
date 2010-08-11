/*******************************************************************************
 *
 * Copyright (c) 2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

template<int>
struct CompileTimeWarningTest;

struct CompileTime{
    template<int>
    struct warning_in_line_{
        enum _ {w};
    };
};

template<bool, typename T1, typename T2>
struct EnumTester {
    typedef EnumTester type;
    enum { w = T1::w == T2::w};
};

template<typename T1, typename T2>
struct EnumTester<true,T1,T2> {
    enum { w = T1::w == T1::w};
};

// 1. Solution which works in all contexts
#define FAMOUSO_STATIC_ASSERT_WARNING(expr, msg, types)                     \
        typedef CompileTimeWarningTest<                                     \
                    sizeof( EnumTester<                                     \
                        expr,                                               \
                        CompileTime::warning_in_line_<__LINE__>,            \
                        msg                                                 \
                    > )                                                     \
                > JOIN(__warning_in_line__,__LINE__)

// 2. Solution which works in not in a function contexts like main
//#define FAMOUSO_STATIC_ASSERT_WARNING(expr, msg, types)                     \
        struct JOIN(JOIN(_,__LINE__),_) {                                   \
                enum msg {w};                                               \
        };                                                                  \
        typedef CompileTimeWarningTest<                                     \
                    sizeof( EnumTester<                                     \
                                expr,                                       \
                                CompileTime::warning_in_line_<__LINE__>,    \
                                JOIN(JOIN(_,__LINE__),_)                    \
                            >)                                              \
                > JOIN(__warning_in_line__,__LINE__)


#define GENERATE_WARNING_MSG(msg)                                           \
struct msg {                                                                \
        enum _ {w};                                                         \
}

GENERATE_WARNING_MSG(WarningInClassTemplateContext);
GENERATE_WARNING_MSG(WarningInFunctionTemplateContext);
GENERATE_WARNING_MSG(GlobalContext);
GENERATE_WARNING_MSG(MainContext);

template <typename t>
struct ClassTemplate {
    FAMOUSO_STATIC_ASSERT_WARNING(true, WarningInClassTemplateContext, () );
    FAMOUSO_STATIC_ASSERT_WARNING(sizeof(t) == 0 && false, WarningInClassTemplateContext, () );
};
ClassTemplate<int> lll;

template <typename t>
void FunctionTemplate () {
    FAMOUSO_STATIC_ASSERT_WARNING(true, WarningInFunctionTemplateContext, () );
    FAMOUSO_STATIC_ASSERT_WARNING(sizeof(t) == 0 && false, WarningInFunctionTemplateContext, () );
}

FAMOUSO_STATIC_ASSERT_WARNING(true, GlobalContext, () );
FAMOUSO_STATIC_ASSERT_WARNING(false, GlobalContext, () );

int main() {
    FAMOUSO_STATIC_ASSERT_WARNING(true, MainContext, () );
    FAMOUSO_STATIC_ASSERT_WARNING(false, MainContext, () );

    FunctionTemplate<long>();
}

