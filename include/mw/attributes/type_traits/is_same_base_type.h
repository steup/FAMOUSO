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

#ifndef __IsSameBaseType_h__
#define __IsSameBaseType_h__

#include <boost/type_traits/is_same.hpp>
#include "config/type_traits/contains_type.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace type_traits {

                /*! \brief The struct implements a test if two types have the
                 *         same base type
                 *
                 *  First it detect if A and B contain a base_type typedef, and
                 *  if so compares whether A and B have the same base_type defined.
                 *
                 * \tparam  A first examined type.
                 * \tparam  B second examined type
                 *
                 * \returns value==1 if base_type is the same value==0
                 *          else
                 */
                template <typename A, typename B>
                struct is_same_base_type {

                    CONTAINS_TYPE_(base_type);

                    template <bool, typename T, typename U>
                    struct is_same_base_type_impl :
                        boost::is_same<
                            typename T::base_type,
                            typename U::base_type
                        > {};

                    template <typename T, typename U>
                    struct is_same_base_type_impl<false,T,U> {
                        enum{
                            value = 0
                        };
                    };

                    typedef is_same_base_type type;

                    enum {
                        value = is_same_base_type_impl<
                                    contains_type_base_type< A >::value
                                        &&
                                    contains_type_base_type< B >::value ,
                                    A,
                                    B
                                >::value
                    };
                };

            } /* type_traits */
        } /* attributes */
    } /* mw */
} /* famouso */

#endif

