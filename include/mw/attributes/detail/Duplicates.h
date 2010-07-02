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

#ifndef _Duplicates_h_
#define _Duplicates_h_

#include "boost/mpl/begin.hpp"
#include "boost/mpl/end.hpp"
#include "boost/mpl/next.hpp"
#include "boost/mpl/deref.hpp"

#include "boost/mpl/eval_if.hpp"
#include "boost/mpl/count_if.hpp"

#include "mw/attributes/type_traits/is_same_base_type.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

                /*!
                 * \brief Can be used to test a given sequence of %attributes for contained
                 *  duplicates.
                 *
                 * A duplicate attribute is determined by having the same base type as its
                 *  regarded pendant. This struct also provides the attribute for which the
                 *  first duplicate weas found.
                 *
                 * \tparam AttrSeq The attribute sequence to test
                 * \tparam Iter The current iterator in the sequence, should be left at
                 *  default when used externally
                 */
                template <typename AttrSeq, typename Iter = typename boost::mpl::begin<AttrSeq>::type>
                struct Duplicates {
                    private:
                        // The current attribute
                        typedef boost::mpl::deref<Iter> curAttr;

                        // A predicate testing every attribute of the sequence against the current
                        //  one based on the base type
                        typedef famouso::mw::attributes::type_traits::is_same_base_type<
                                                                            boost::mpl::_1,
                                                                            typename curAttr::type
                                                                      > pred;

                        // The number of attributes in the sequence matching the current one
                        typedef typename boost::mpl::count_if<AttrSeq, pred> currentCount;

                        // The iterator pointing to the next attribute in the sequence
                        typedef boost::mpl::next<Iter> nextIter;

                        // The Duplicates struct using the next iterator
                        typedef Duplicates<AttrSeq, typename nextIter::type> nextDupl;

                    public:
                        /*!
                         * \brief True, if the given sequence contains duplicates, false
                         *  otherwise
                         */
                        static const bool result = (currentCount::value > 1) ? true : nextDupl::result;

                        /*!
                         * \brief The attribute for which the first duplicate was found.
                         */
                        typedef typename boost::mpl::eval_if_c<
                                                      result,
                                                      typename curAttr::type,
                                                      typename nextDupl::duplicateAttribute
                                                     >::type duplicateAttribute;
                };

                /*!
                 * \brief End-of-recursion struct
                 */
                template <typename AttrSeq>
                struct Duplicates<AttrSeq, typename boost::mpl::end<AttrSeq>::type> {
                    private:
                        struct Dummy {
                            typedef Dummy type;
                        };

                    public:
                        // \todo: Could anyone please tell me why a static const member is
                        //  not "constant enough" so that I must use this ugly enum?!
                        // (and why is it sufficient in the general template definition above?!)
                        enum {
                            result = false
                        };

                        typedef Dummy duplicateAttribute;
                };

            }  // end namespace detail
        }  // end namespace attributes
    }  // end namespace mw
}  // end namespace famouso

#endif
