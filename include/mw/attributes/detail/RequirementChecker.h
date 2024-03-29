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
#include "boost/mpl/integral_c.hpp"
#include "boost/mpl/bool.hpp"

#include "boost/mpl/filter_view.hpp"

#include "mw/attributes/type_traits/is_null.h"
#include "mw/attributes/detail/AttributeCompileErrors.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

                /*!
                 * \brief Predicate to filter out non-requirable attributes from
                 *  a requirement specification
                 */
                struct RequirablePred {
                    template <typename Attr>
                    struct apply : boost::mpl::bool_<Attr::requirable> {};
                };

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
                          typename FilteredReq = typename boost::mpl::filter_view<
                                                  typename Req::sequence,
                                                  RequirablePred
                                                 >::type,
                          typename ReqIter = typename boost::mpl::begin<FilteredReq>::type
                         >
                struct RequirementChecker {
                    private:
                        // TODO: Rework the requirable assertion so that a more simply error message
                        //  is generated (e.g. by using the invert predicate of RequirablePred)
                        template <typename Pr, typename PrIter = typename boost::mpl::begin<Pr>::type>
                        struct provision_assertion {
                            private:
                                typedef typename boost::mpl::deref<PrIter>::type curProvAttr;

                                FAMOUSO_STATIC_ASSERT_ERROR(
                                        (curProvAttr::requirable),
                                        provision_contains_non_requirable_attribute,
                                        (curProvAttr, Pr));

                            public:
                                typedef typename provision_assertion<
                                                  Pr,
                                                  typename boost::mpl::next<PrIter>::type
                                                 >::type type;
                        };
                        template <typename Pr>
                        struct provision_assertion<Pr, typename boost::mpl::end<Pr>::type> {
                                typedef provision_assertion type;
                        };

                        typedef typename provision_assertion<
                                          typename Prov::sequence
                                         >::type assertProvision;

                        /*!
                         * \brief The current attribute of the requirement, determined
                         *  by the iterator.
                         */
                        typedef typename boost::mpl::deref<ReqIter>::type curAttr;

                        /*!
                         * \brief The provision attribute which is equal to the current
                         *  requirement attribute.
                         */
                        typedef typename Prov::template find_ct<curAttr>::type provAttr;

                        /*!
                         * \brief Determines whether the current attribute is not contained
                         *  in the provision.
                         */
                        typedef famouso::mw::attributes::type_traits::is_null<
                                                                       provAttr
                                                                      > notContained;

                        /*!
                         * \brief The comparator used for checking the values of the
                         *  attributes.
                         *
                         * It is defined by the attribute itself in the meaning of a
                         *  stronger relation. Only requireable attributes may be used
                         *  here, this is asserted by the attribute's isStronger struct.
                         */
                        template <typename Attribute, bool dummy>
                        struct comparatorExtractor {
                                typedef typename Attribute::template isStronger<curAttr> compare;
                        };
                        template <bool dummy>
                        struct comparatorExtractor<famouso::mw::attributes::Null, dummy> {
                                // We let this default to true to not issue two errors if a
                                //  required attribute is not found
                                typedef boost::mpl::bool_<true> compare;
                        };

                        /*!
                         * \brief Determines whether the attribute values fit and therefore
                         *  the system can be composed without violating the requirement.
                         */
                        static const bool valueFits = comparatorExtractor<provAttr, false>::compare::value;

                        typedef typename AttributeIsNotRequireable<
                                          curAttr::requirable, curAttr, Req, compileError
                                         >::type assert1;

                        // Only issue the "attribute is not contained..." error if the error regarding
                        //  requirable attributes was not issued
                        static const bool issueNotContainedError = (notContained::value && curAttr::requirable);

                        /*!
                         * Issue a compiler error if the current attribute is
                         *  not contained in the provision.
                         */
                        typedef typename RequiredAttributeNotContainedInProvision<
                                         !issueNotContainedError , curAttr, Prov, compileError
                                         >::type assert2;

                        /*!
                         * Issue a compiler error if the values of the related attributes do
                         *  not fit
                         */
                        typedef typename RequiredValueNotProvided<
                                          valueFits, curAttr, provAttr, compileError
                                         >::type assert3;

                        /*!
                         * \brief Instantiation of the checker instance responsible for the
                         *  next attribute in the requirement.
                         */
                        typedef typename RequirementChecker<
                                          Prov,
                                          Req,
                                          compileError,
                                          FilteredReq,
                                          typename boost::mpl::next<ReqIter>::type
                                         >::result nextResult;
                    public:
                        /**!
                         * \brief This type.
                         */
                        typedef RequirementChecker type;

                        typedef boost::mpl::integral_c_tag tag;

                        typedef bool value_type;

                        typedef typename boost::mpl::eval_if_c<
                                                      (!notContained::value && valueFits),
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

                        operator bool() const {
                            return (this->value);
                        }
                };

                template <typename Prov, typename Req, typename FilteredReq>
                struct RequirementChecker<
                        Prov,
                        Req,
                        true,
                        FilteredReq,
                        typename boost::mpl::end<FilteredReq>::type
                       > : public boost::mpl::integral_c<bool, true> {
                    public:
                        typedef RequirementChecker type;

                        typedef boost::mpl::bool_<true> result;
                };

                template <typename Prov, typename Req, typename FilteredReq>
                struct RequirementChecker<
                        Prov,
                        Req,
                        false,
                        FilteredReq,
                        typename boost::mpl::end<FilteredReq>::type
                       > : public boost::mpl::integral_c<bool, true> {
                    public:
                        typedef RequirementChecker type;

                        typedef boost::mpl::bool_<true> result;
                };

            }  // namespace detail
        }  // namespace attributes
    }  // namespace mw
}  // namespace famouso

#endif
