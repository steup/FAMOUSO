/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Marcus Foerster <MarcusFoerster1@gmx.de>
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

#ifndef _Tag_Set_
#define _Tag_Set_

#include "boost/mpl/aux_/na.hpp"
#include "boost/type_traits/is_same.hpp"
#include "boost/mpl/if.hpp"
#include "boost/mpl/contains.hpp"

#include "boost/mpl/vector.hpp"

#include "assert/staticerror.h"
#include "mw/attributes/detail/tags/Tag.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

                template <typename T>
                struct assert_is_tag_or_na {
                        static const bool cond = (is_tag<T>::value) ||
                                (boost::is_same<T, boost::mpl::na>::value);

                        FAMOUSO_STATIC_ASSERT_ERROR(cond,
                                                    only_tags_or_na_allowed,
                                                    (T));

                        typedef assert_is_tag_or_na type;
                };

                template <typename T1 = boost::mpl::na, typename T2 = boost::mpl::na,
                          typename T3 = boost::mpl::na, typename T4 = boost::mpl::na,
                          typename T5 = boost::mpl::na, typename T6 = boost::mpl::na,
                          typename T7 = boost::mpl::na, typename T8 = boost::mpl::na,
                          typename T9 = boost::mpl::na, typename T10 = boost::mpl::na>
                struct TagSet {

                    private:
                        typedef typename assert_is_tag_or_na<T1>::type  assert1;
                        typedef typename assert_is_tag_or_na<T2>::type  assert2;
                        typedef typename assert_is_tag_or_na<T3>::type  assert3;
                        typedef typename assert_is_tag_or_na<T4>::type  assert4;
                        typedef typename assert_is_tag_or_na<T5>::type  assert5;
                        typedef typename assert_is_tag_or_na<T6>::type  assert6;
                        typedef typename assert_is_tag_or_na<T7>::type  assert7;
                        typedef typename assert_is_tag_or_na<T8>::type  assert8;
                        typedef typename assert_is_tag_or_na<T9>::type  assert9;
                        typedef typename assert_is_tag_or_na<T10>::type assert10;

                        // TODO: Find out how to sort tags (they need some kind of integral
                        //  ID or something like that)
                        typedef typename boost::mpl::vector<
                                          T1, T2, T3, T4, T5, T6, T7, T8, T9, T10
                                         >::type tag_vector;

                    public:
                        typedef TagSet type;

                        template <typename T>
                        struct contains_tag : boost::mpl::contains<tag_vector, T> { };
                };

            } // end namespace detail
        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif // _Tag_Set_
