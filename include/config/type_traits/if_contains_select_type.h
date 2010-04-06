/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
 *                    Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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


#ifndef __IF_CONTAINS_SELECT_TYPE_H_62F83DCA2DD154__
#define __IF_CONTAINS_SELECT_TYPE_H_62F83DCA2DD154__


#include "config/type_traits/if_select_type.h"
#include "config/type_traits/contains_type.h"


/*!
 *  \brief  This macro generates a compile time testing class which
 *          selects a primary type if it is defined within a type/class.
 *          If it is not defined there, a secondary class is selected.
 *
 *  \param[in]  NAME Type we are looking for inside a type/class.
 *
 *  \par Usage:
 *      The macro defines a template class with two type parameters. The
 *      first (\p T) is the type which may contain a definition of type
 *      \p NAME. If it contains a definition, the type member is defined
 *      as \c T::NAME. Otherwise it's the second type parameter
 *      (\p Alternative).
 *
 * \par Example:
\code
struct TypeWithTestType {
    typedef int TestType;
};
struct TypeWithoutTestType {
};

IF_CONTAINS_SELECT_TYPE_(TestType);

if_contains_select_type_TestType<TypeWithoutTestType, short>::type a; // type of a is short
if_contains_select_type_TestType<TypeWithTestType, short>::type b;    // type of b is TypeWithTestType::TestType
\endcode
 *
 *  \hideinitializer
 */
#define IF_CONTAINS_SELECT_TYPE_(NAME)                                      \
template < typename T, typename Alternative>                                \
class if_contains_select_type_##NAME {                                      \
        CONTAINS_TYPE_(NAME);                                               \
        struct AlternativeParent {                                          \
            typedef Alternative NAME;                                       \
        };                                                                  \
        typedef typename if_select_type<                                    \
                    contains_type_##NAME<T>::value,                         \
                    T,                                                      \
                    AlternativeParent                                       \
                >::type parent_type;                                        \
    public:                                                                 \
        typedef typename parent_type::NAME type;                            \
}


#endif // __IF_CONTAINS_SELECT_TYPE_H_62F83DCA2DD154__

