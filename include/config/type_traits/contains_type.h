/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
#ifndef __contains_type_h__
#define __contains_type_h__

/*! \brief the CONTAINS_TYPE_(NAME) macro generates a compile time testing
 *         class, that allows for checking whether a type is defined within a
 *         type/class like a configuration description.
 *
 *  \param[in] NAME is the name of the subtype, that has to be tested.
 *
 *         The checking is complete done at compile time, thus no resources
 *         are wasted. It works with overloading of functions that have
 *         different return types and signatures and using SFINAE. Dependend
 *         of the return type and using the sizeof-trick we are able to
 *         detect whether a type is specified as a subtype or not.
 *
 *         contains_type_NAME<T>::value returns 1 if \a NAME exicist within
 *         \a T as defined subtype and 0 else.
 *
 *         How to use it? Here is an example:
 *
\code
struct TypeWithTestType {
    typedef int TestType;
};

CONTAINS_TYPE_(TestType);

contains_type_TestType<int>::value;                 // returns 0
contains_type_TestType<TypeWithTestType>:: value    // returns 1

\endcode
 *
 *  \author Michael Schulze
 *
 *  \hideinitializer
 */
#define CONTAINS_TYPE_(NAME)                                                \
template < typename V>                                                      \
class contains_type_##NAME {                                                \
        /* auxiliary types */                                               \
        typedef char NoType;                                                \
        class YesType {                                                     \
            NoType dummy[2];                                                \
        };                                                                  \
                                                                            \
        struct Tester {                                                     \
            /* fits on all types */                                         \
            static inline NoType Test(...);                                 \
            /* fits on special types that have type NAME included */        \
            template<typename U>                                            \
            static inline YesType Test(U* const volatile,                   \
                                       typename U::NAME *t1 = 0 );          \
        };                                                                  \
                                                                            \
    public:                                                                 \
        /* the value signalise if the subtype is defined or not   */        \
        static const bool value = (                                         \
                                   sizeof(YesType)                          \
                                        ==                                  \
                                   sizeof(Tester::Test(static_cast<V*>(0))) \
                                  );                                        \
                                                                            \
}

#endif

