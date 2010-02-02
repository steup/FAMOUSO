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

#ifndef __RelationalOperatorFilterReturnTypeCalculator_h__
#define __RelationalOperatorFilterReturnTypeCalculator_h__

#include <boost/mpl/not.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/type_traits/is_integral.hpp>

#include "mw/attributes/CompileErrors.h"

#include "mw/attributes/tags/AttributeTag.h"
#include "mw/attributes/tags/SameTag.h"
#include "mw/attributes/tags/CompareDifferentAttributesTag.h"
#include "mw/attributes/tags/CompileTimeFilterTag.h"

#include "mw/attributes/type_traits/is_compare_tag.h"
#include "mw/attributes/type_traits/is_type_tag.h"
#include "mw/attributes/type_traits/is_same_base_type.h"

#include "mw/attributes/IntegralConstType.h"
#include "mw/attributes/IntegralConstRunTimeWrapper.h"

#include "mw/attributes/filter/RelationalOperatorFilter.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace filter {

            /*! \brief The struct implements a return type calculator for
             *          %filter expression.
             *
             * It is used as return type calculator on free relational
             * operators like >, <, etc. It calculates not only return types of
             * %filter expression but also detects violations against the
             * %filter grammar by checking attribute requirements and
             * constraints.
             *
             * \tparam  A is an attribute, and it has to define the
             *          attribute_tag type. It has to be conform with the
             *          attribute description of the attribute grammar.
             *
             * \tparam  B can be of type integral run-time value, integral
             *          constant, ordinary attribute type or of an attribute
             *          type with special conditions. B needs to fulfil the
             *          requirements of A.
             *
             * \returns a compile error or a valid return type in dependance
             *          whether the expression meets the attribute grammar or
             *          not.
             */
            template <typename A, typename B, typename Operator=void>
            struct RelationalOperatorFilterReturnTypeCalculator :
                // test whether A meets the attribute grammar
                boost::mpl::eval_if<
                    boost::mpl::not_<
                        type_traits::is_type_tag<
                            A,
                            tags::attribute_tag
                        >
                    >,
                    // A. meets not the attribute grammar and leading to a
                    // compile error.
                    CompileError<
                        A,
                        B,
                        tags::attribute_tag
                    >,
                    // A meets the attribute grammar
                    boost::mpl::eval_if<
                        //  Requires A. the same type for comparisions?
                        type_traits::is_compare_tag<
                            A,
                            tags::same_tag
                        >,
                        // If yes, B needs to have the same base type.
                        boost::mpl::eval_if<
                            // test whether B meets the attribute grammar
                            boost::mpl::not_<
                                type_traits::is_type_tag<
                                    B,
                                    tags::attribute_tag
                                >
                            >,
                            // B. meets not the attribute grammar and leading
                            // to a compile error.
                            CompileError<B, tags::attribute_tag>,
                            // B meets the attribute grammar and now test
                            // whether it fulfils A requirements
                            boost::mpl::eval_if<
                                type_traits::is_same_base_type< A, B >,
                                //
                                // \todo
                                // compare_ mit entsprechendem Operator
                                // hier ist auch compile time moeglich
                                RelationalOperatorFilter<
                                    A,
                                    B,
                                    Operator,
                                    tags::compile_time_filter_tag
                                >,
                                // A requires the same_tag and B does not meets
                                // the requirements
                                CompileError<A, B, tags::same_tag>
                            >
                        >,
                        // A is an ordinary attribute and allows comparisions
                        // with intergrals, integral constants and with A types
                        boost::mpl::eval_if<
                            boost::is_integral<B>,
                            // compare_dyn mit entsprechendem Operator
                            RelationalOperatorFilter<
                                A,
                                IntegralConstRunTimeWrapper< B >,
                                Operator
                            >,
                            boost::mpl::eval_if<
                                boost::mpl::not_<
                                    type_traits::is_type_tag<
                                        B,
                                        tags::attribute_tag
                                    >
                                >,
                                // B does not meets the attribute grammar
                                CompileError< B, tags::attribute_tag>,
                                // test on attribute grammar compliance
                                boost::mpl::eval_if<
                                    boost::mpl::or_<
                                        type_traits::is_same_base_type<
                                            A,
                                            B
                                        >,
                                        type_traits::is_same_base_type<
                                            IntegralConstType,
                                            B
                                        >
                                    >,
                                    // compare_ mit entsprechendem Operator
                                    RelationalOperatorFilter<
                                        A,
                                        B,
                                        Operator,
                                        tags::compile_time_filter_tag
                                    >,
                                    // comparisions of different attribute
                                    // types is not allowed by the attribute
                                    // grammar
                                    CompileError<
                                        A,
                                        B,
                                        tags::compare_different_attributes_tag
                                    >
                                >
                            >
                        >
                    >
                > {};

            } /* filter */
        } /* attributes */
    } /* mw */
} /* famouso */

#endif

