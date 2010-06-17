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

#ifndef _RequirementChecker_h_
#define _RequirementChecker_h_

#include "boost/mpl/begin.hpp"
#include "boost/mpl/end.hpp"
#include "boost/mpl/deref.hpp"
#include "boost/mpl/next.hpp"
#include "boost/mpl/eval_if.hpp"

#include "mw/attributes/detail/FindStatic.h"
#include "mw/attributes/detail/AttributeCompileErrors.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

                /*!
                 * \brief Checks the given attribute requirement against the given
                 *  provision. The check result can be accessed using the result
                 *  type.
                 *
                 * It can be configured if this struct should generate a compile
                 *  error finding the first violated condition. In this case result
                 *  will always be boost::mpl::bool_<true> of course. However the
                 *  compile error generation can be turned off just generating
                 *  warnings in case of a condition violation.
                 *
                 * \tparam Prov The attribute set representing the provision
                 * \tparam Req The attribute set representing the requirement
                 * \tparam compileError Switch to turn compile errors on and off
                 * \tparam ReqIter The iterator to the current requirement
                 *  attribute, should be left at default from the outside
                 */
                template <typename Prov, typename Req, bool compileError = true,
                          typename ReqIter = typename boost::mpl::begin<typename Req::sequence>::type>
                struct RequirementChecker {
                    private:
                        /*!
                         * \brief The current attribute of the requirement, determined
                         *  by the iterator.
                         */
                        typedef typename boost::mpl::deref<ReqIter>::type curAttr;

                        /*!
                         * \brief Determines whether the current attribute is contained
                         *  in the provision.
                         */
                        typedef typename IsContained<
                                          curAttr,
                                          typename Prov::sequence
                                         >::result contained;

                        /*!
                         * Issue a compiler error if the current attribute is
                         *  not contained in the provision.
                         */
                        typedef typename RequiredAttributeNotContainedInProvision<
                                          contained::value, curAttr, Prov, compileError
                                         >::type assert1;

                        /*!
                         * \brief The provision attribute which is equal to the current
                         *  requirement attribute.
                         */
                        typedef typename Find<
                                 typename curAttr::type,
                                 typename Prov::sequence
                                >::result provAttr;

                        typedef typename curAttr::type::comparator::template apply_<
                                                                     provAttr,
                                                                     curAttr
                                                                    > compare;

                        /*!
                         * \brief Determines whether the attribute values fit and therefore
                         *  the system can be composed without violating the requirement.
                         */
                        static const bool valueFits = compare::value;

                        /*!
                         * Issue a compiler error if the values of the related attributes do
                         *  not fit
                         */
                        typedef typename RequiredValueNotProvided<
                                          valueFits, curAttr, provAttr, compileError
                                         >::type assert2;

                        /*!
                         * \brief Instantiation of the checker instance responsible for the
                         *  next attribute in the requirement.
                         */
                        typedef typename RequirementChecker<
                                          Prov,
                                          Req,
                                          compileError,
                                          typename boost::mpl::next<ReqIter>::type
                                         >::result nextResult;
                    public:
                        /**!
                         * \brief This type.
                         */
                        typedef RequirementChecker type;

                        typedef typename boost::mpl::eval_if_c<
                                                      (contained::value && valueFits),
                                                      nextResult,
                                                      boost::mpl::bool_<false>
                                                     >::type result;

                        /*!
                         * \brief The final result for the check of the whole requirement
                         *  against the given provision.
                         *
                         * It is true if and only if every attribute of the requirement
                         *  can be found in the provision and each of these pairs have
                         *  fitting values.
                         */
                        static const bool value = result::value;
                };

                template <typename Prov, typename Req>
                struct RequirementChecker<
                        Prov,
                        Req,
                        true,
                        typename boost::mpl::end<typename Req::sequence>::type
                       > {
                    public:
                        typedef RequirementChecker type;

                        typedef boost::mpl::bool_<true> result;

                        static const bool value = true;
                };

                // TODO: Since default template parameters cannot be used in a partial
                //  specialization, we have to declare both "compileError"-cases
                //  separately (fortunately it's just a boolean ;))

                template <typename Prov, typename Req>
                struct RequirementChecker<
                        Prov,
                        Req,
                        false,
                        typename boost::mpl::end<typename Req::sequence>::type
                       > {
                    public:
                        typedef RequirementChecker type;

                        typedef boost::mpl::bool_<true> result;

                        static const bool value = true;
                };

            }  // namespace detail
        }  // namespace attributes
    }  // namespace mw
}  // namespace famouso

#endif
