/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Marcus Förster <MarcusFoerster1@gmx.de>
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

#ifndef _STATIC_WARNING_H_
#define _STATIC_WARNING_H_

#define JOIN( X, Y ) DO_JOIN( X, Y )
#define DO_JOIN( X, Y ) DO_JOIN2(X,Y)
#define DO_JOIN2( X, Y ) X##Y

#include "boost/mpl/eval_if.hpp"

namespace failed_assertion {

    template<int>
    struct CompileTimeWarningTest;

    struct args;
}

#if defined __GNUC__

namespace failed_assertion {

    struct Integer {
        typedef Integer type;
        static const unsigned int w=8;
    };


    template<typename>
    struct _warning_ {
        typedef _warning_ type;
        enum _ {w};
    };

}
#define FAMOUSO_STATIC_ASSERT_WARNING(expr, msg, types)                     \
    struct JOIN(JOIN(_failed_assertion_in_line_,__LINE__),_with_message_) { \
        enum JOIN(JOIN(__,msg),__) {w};                                     \
        static void args types { }                                          \
    };                                                                      \
    typedef ::failed_assertion::CompileTimeWarningTest<                     \
                JOIN(                                                       \
                    JOIN(                                                   \
                        _failed_assertion_in_line_,__LINE__                 \
                    ),                                                      \
                    _with_message_                                          \
                )::w                                                        \
                    ==                                                      \
                boost::mpl::eval_if_c<                                      \
                    expr,                                                   \
                    ::failed_assertion::Integer,                            \
                    ::failed_assertion::_warning_<                          \
                        void**** (::failed_assertion::args::****) types     \
                    >                                                       \
                >::type::w                                                  \
            > JOIN(__warning_in_line__,__LINE__)

#elif defined _MSC_VER

namespace failed_assertion {

    template<typename,bool>
    struct _warning_ {
        typedef _warning_ type;
    };
}

#define FAMOUSO_STATIC_ASSERT_WARNING(expr, msg, types)                     \
    __pragma(warning (push, 1))                                             \
    __pragma(warning (1:4263))                                              \
    struct JOIN(_failed_assertion_line_,__LINE__)              {            \
        struct _ {                                                          \
            virtual void _ ## msg ## _( ::failed_assertion::_warning_<      \
                        void**** (::failed_assertion::args::****) types,    \
                        true                                                \
                   >) = 0;                                                  \
        };                                                                  \
        struct __ : _ {                                                     \
            void _ ## msg ## _(::failed_assertion::_warning_<               \
                        void**** (::failed_assertion::args::****) types,    \
                        (expr)                                              \
                   > );                                                     \
        };                                                                  \
        typedef ::failed_assertion::CompileTimeWarningTest<sizeof(__)> ___; \
    };                                                                      \
    __pragma(warning (pop))                                                 \
    typedef ::failed_assertion::CompileTimeWarningTest<                     \
                sizeof(JOIN(_failed_assertion_line_,__LINE__))              \
            > JOIN(__,__LINE__)


#else

#warning "Compiler is not supported by the FAMOUSO_STATIC_ASSERT_WARNING macro"
#define FAMOUSO_STATIC_ASSERT_WARNING(expr, msg, types)

#endif

#endif
