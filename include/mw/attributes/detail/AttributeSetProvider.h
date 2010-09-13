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

#ifndef _Set_Provider_h_
#define _Set_Provider_h_

#include "boost/mpl/aux_/na.hpp"
#include "boost/utility/enable_if.hpp"
#include "boost/mpl/vector.hpp"
#include "boost/mpl/is_sequence.hpp"
#include "boost/mpl/sort.hpp"
#include "boost/mpl/eval_if.hpp"
#include "boost/mpl/bool.hpp"

#include "mw/attributes/AttributeSet.h"
#include "mw/attributes/type_traits/is_attribute.h"

#include "assert/staticerror.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {
                /*!
                 * \brief Helper struct to assert that a given type is an
                 *  attribute or na
                 *
                 * \tparam Attr The type to test
                 * \tparam Enable A switch to distinguish %attributes from
                 *  the rest
                 */
                template <typename Attr, typename InnerEnable = void>
                struct assert_is_attribute_or_na  {
                        FAMOUSO_STATIC_ASSERT_ERROR(
                            false,
                            template_argument_no_attribute_and_not_na,
                            (Attr));

                        typedef void type;
                };

                template <>
                struct assert_is_attribute_or_na<boost::mpl::na, void> {
                        typedef void type;
                };

                template <typename Attr>
                struct assert_is_attribute_or_na<
                        Attr,
                        typename boost::enable_if<
                                        famouso::mw::attributes::type_traits::is_attribute<
                                                                               Attr
                                                                              >
                                       >::type
                       > {
                        typedef void type;
                };

                template <typename T>
                struct is_sequence : boost::mpl::is_sequence<T> { };

                template <>
                struct is_sequence<boost::mpl::na> {
                        typedef is_sequence type;

                        static const bool value = false;
                };

                struct SorterPredicate {
                        // For both types being attributes
                        template <typename A1, typename A2>
                        struct apply {
                                static const bool value = A1::id > A2::id;

                                typedef apply type;
                        };

                        // For only the first type being an attribute
                        template <typename T1>
                        struct apply<T1, boost::mpl::na> {
                                static const bool value = true;

                                typedef apply type;
                        };

                        // For only the second type being an attribute
                        template <typename T2>
                        struct apply<boost::mpl::na, T2> : public apply<T2, boost::mpl::na> {};

                        typedef SorterPredicate type;
                };

                template <typename Seq>
                struct SortedAttributeSequence {
                        typedef typename boost::mpl::sort<Seq, SorterPredicate>::type result;

                        typedef SortedAttributeSequence type;
                };

                /*!
                 * \brief Represents the inner implementation of the attribute sequence
                 *  part of an ExtendedEvent.
                 *
                 * The struct provides 10 (actually 11, the last one is a kind of switch)
                 *  type arguments which can either all be attributes or only the first
                 *  of them a forward sequence as defined by the Boost MPL library.
                 *
                 * A compile time assertion assures that only these two cases can be
                 *  instantiated.
                 *
                 * It is also legal to provide none of the template parameters at all.
                 *
                 * Both variants lead to an AttributeSequence type defined containing
                 *  either the single attributes or the elements of the provuides
                 *  sequence.
                 *
                 * \tparam A1 The first attribute or the forward sequence
                 * \tparam A2 The second attribute or nothing
                 * \tparam A3 The third attribute or nothing
                 * \tparam A4 The fourth attribute or nothing
                 * \tparam A5 The fifth attribute or nothing
                 * \tparam A6 The sixth attribute or nothing
                 * \tparam A7 The seventh attribute or nothing
                 * \tparam A8 The eighth attribute or nothing
                 * \tparam A9 The ninth attribute or nothing
                 * \tparam A10 The tenth attribute or nothing
                 *
                 * \tparam Enable A switch to distinguish between both cases
                 */
                template <typename A1 = boost::mpl::na, typename A2 = boost::mpl::na,
                          typename A3 = boost::mpl::na, typename A4 = boost::mpl::na,
                          typename A5 = boost::mpl::na, typename A6 = boost::mpl::na,
                          typename A7 = boost::mpl::na, typename A8 = boost::mpl::na,
                          typename A9 = boost::mpl::na, typename A10 = boost::mpl::na,
                          typename Enable = void>
                struct SetProvider {
                        // Primary template implements the case that all given types
                        //  are attributes

                    private:
                        // Apply the assertion to all of the given types

                        typedef typename assert_is_attribute_or_na<A1>::type  aa1;
                        typedef typename assert_is_attribute_or_na<A2>::type  aa2;
                        typedef typename assert_is_attribute_or_na<A3>::type  aa3;
                        typedef typename assert_is_attribute_or_na<A4>::type  aa4;
                        typedef typename assert_is_attribute_or_na<A5>::type  aa5;
                        typedef typename assert_is_attribute_or_na<A6>::type  aa6;
                        typedef typename assert_is_attribute_or_na<A7>::type  aa7;
                        typedef typename assert_is_attribute_or_na<A8>::type  aa8;
                        typedef typename assert_is_attribute_or_na<A9>::type  aa9;
                        typedef typename assert_is_attribute_or_na<A10>::type aa10;

                        // If none of the assertions was violated, define a vector
                        //  containing the given attribute types
                        typedef typename SortedAttributeSequence<
                                          boost::mpl::vector<A1, A2, A3, A4, A5,
                                                   A6, A7, A8, A9, A10
                                          >
                                         >::result attribs;

                    public:
                        /*!
                         * \brief The provided attribute sequence
                         */
                        typedef attributes::AttributeSet<attribs> attrSet;
                };

                template <typename ForwardSeq>
                struct SetProvider<ForwardSeq, boost::mpl::na, boost::mpl::na,
                                        boost::mpl::na, boost::mpl::na, boost::mpl::na,
                                        boost::mpl::na, boost::mpl::na, boost::mpl::na,
                                        boost::mpl::na,
                                        typename boost::enable_if<
                                                         is_sequence<ForwardSeq>
                                                        >::type> {
                        // Specialized template cares for the case that only a forward
                        //  sequence of attributes is given as the first type

                        typedef typename SortedAttributeSequence<
                                          ForwardSeq
                                         >::result attribs;

                    public:
                        /*!
                         * \brief The provided attribute sequence
                         */
                        typedef attributes::AttributeSet<attribs> attrSet;
                };

            }  // end namespace detail
        }  // end namespace attributes
    }  // end namespace mw
}  // end namespace famouso

#endif // _Set_Provider_
