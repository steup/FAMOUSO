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

#ifndef __AttributesFramework_h__
#define __AttributesFramework_h__

#define F_CPU 16000000
#include "debug.h"
#include <typeinfo>
#define BOOST_NO_STDLIB_CONFIG
#include  <stdint.h>

#include  <boost/type_traits/is_same.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/static_assert.hpp>
#include <boost/mpl/iterator_tags.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/prior.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/advance.hpp>
#include <boost/mpl/distance.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/minus.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/clear.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/mpl/less.hpp>
#include <boost/mpl/less_equal.hpp>
#include <boost/mpl/void.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/pop_back.hpp>
#include <boost/mpl/erase.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/count.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/equal_to.hpp>


namespace famouso {
    namespace mw {
        namespace Attributes {
            namespace detail {

                // Test on the tag of a type because different template parameter on an
                // attribute type leads to different types but the tag_type does not change
                template<typename X, typename Y>
                struct test_same : boost::is_same <
                                            typename X::type::tag_type,
                                            typename Y::tag_type
                                        > {};

                // traverse a sequence of types and detects duplicates
                // if value == 0 duplicates detected value == 1 else
                template < class S, class I = typename boost::mpl::begin<S>::type >
                struct TestOnDuplicates {
                    typedef TestOnDuplicates type;
                    enum {
                        value = boost::mpl::if_ <
                                    boost::mpl::equal_to <
                                        boost::mpl::int_ <
                                            // test each element of the sequence against each other
                                            // and if value > 1 we have detected a duplicate
                                            boost::mpl::count_if <
                                                S,
                                                test_same <
                                                    boost::mpl::_1,
                                                    typename boost::mpl::deref<I>::type
                                                >
                                            >::value
                                        > ,
                                        boost::mpl::int_<1>
                                    > ,
                                    // test next element
                                    TestOnDuplicates <
                                        S,
                                        typename boost::mpl::next<I>::type
                                    > ,
                                    // the if clause detected a duplicate and now return zero
                                    boost::mpl::int_<0>
                                >::type::value
                    };

                    // print the sequence
                    static void traverse() {
                        ::logging::log::emit() << " Type " << typeid(typename boost::mpl::deref<I>::type).name() << ::logging::log::endl;
                        TestOnDuplicates <
                            S,
                            typename boost::mpl::next<I>::type
                        >::traverse();
                    }
                };

                // template/rule for the end of the recursion
                template <class S>
                struct TestOnDuplicates <S, typename boost::mpl::end<S>::type> {
                    typedef TestOnDuplicates type;
                    enum {
                        value = 1
                    };
                    static void traverse() {
                        ::logging::log::emit() << "End of Sequence" << ::logging::log::endl;
                    }
                };

                // definition that allows for switching on/off the compile time error generation
                template <typename S, int, bool b>
                struct CompileTimeErrorGenerationRepeatedAttributes {
                    typedef CompileTimeErrorGenerationRepeatedAttributes type;
                };

                template <typename S, int value>
                struct CompileTimeErrorGenerationRepeatedAttributes <S, value, true> {
                    typedef CompileTimeErrorGenerationRepeatedAttributes type;
                    BOOST_MPL_ASSERT_MSG((value == 1), REPEATED_ATTRIBUTES, (S));
                };

                // define an Tester that detects Duplicate Attributes
                // It is configurable, whether the compiler should generate compile time errors
                // on repeated attributes or it only should count they.
                template < typename T, bool CompileTimeTest = true >
                struct DuplicateAttributes {
                    typedef DuplicateAttributes type;
                    enum {
                        value = famouso::mw::Attributes::detail::TestOnDuplicates< T >::value
                    };
                    typedef typename CompileTimeErrorGenerationRepeatedAttributes<T, value, CompileTimeTest>::type CTT;
                };


                // definition that allows for switching on/off the compile time error generation
                // for attribute mismatches
                template <typename S, int, bool b>
                struct CompileTimeErrorGenerationAttributesMismatch {
                    typedef CompileTimeErrorGenerationAttributesMismatch type;
                };

                template <typename S, int value>
                struct CompileTimeErrorGenerationAttributesMismatch <S, value, true> {
                    typedef CompileTimeErrorGenerationAttributesMismatch type;
                    BOOST_MPL_ASSERT_MSG((value == 1), ATTRIBUTES_MISMATCH, (S));
                };

                // define an Tester that detects Duplicate Attributes
                // It is configurable, whether the compiler should generate compile time errors
                // on repeated attributes or it only should count they.
                template<typename A1, typename A2, bool CompileTimeErrorSignaler=true >
                struct TestAttributes {
                    typedef TestAttributes type;
                    enum {
                        value = (A1::type::value <= A2::type::value)
                    };
                    typedef typename CompileTimeErrorGenerationAttributesMismatch<
                                         boost::mpl::list2< typename A1::type , typename A2::type >,
                                         value,
                                         CompileTimeErrorSignaler
                                     >::type CTT;
                };

            }
        }
    }
}

namespace famouso {
    namespace mw {
        namespace Attributes {

            template < bool CompileTimeErrorSignaler=true >
            struct Properties {
                template < typename T0 = boost::mpl::na, typename T1 = boost::mpl::na, typename T2 = boost::mpl::na,
                           typename T3 = boost::mpl::na, typename T4 = boost::mpl::na, typename T5 = boost::mpl::na,
                           typename T6 = boost::mpl::na, typename T7 = boost::mpl::na, typename T8 = boost::mpl::na >
                struct List {
                    typedef List type;
                    typedef typename boost::mpl::list<T0, T1, T2, T3, T4, T5, T6, T7, T8>::type list_type;
                    typedef typename famouso::mw::Attributes::detail::DuplicateAttributes<
                                         list_type,
                                         CompileTimeErrorSignaler
                                     >::type duplicates;

                    static void print_elements() {
                        famouso::mw::Attributes::detail::TestOnDuplicates<list_type>::traverse();
                    }


                    template<typename A, typename S=type, typename list = typename S::list_type>
                    struct CheckAttributeAgainstList {
                        typedef CheckAttributeAgainstList type;
                        typedef typename boost::mpl::find_if <
                                            list,
                                            famouso::mw::Attributes::detail::test_same<boost::mpl::_1, A>
                                         >::type iter;

                        // search within the sequence if an attribute of the
                        // same type exists.  If yes, then test the attributes
                        //
                        // here also the possibility exists for switching
                        // between compile time error generation or not.
                        typedef typename boost::mpl::eval_if <
                                            boost::is_same <
                                                typename boost::mpl::end<S>::type,
                                                iter
                                            >,
                                            boost::mpl::int_<1>,
                                            famouso::mw::Attributes::detail::TestAttributes<
                                                A,
                                                typename boost::mpl::deref<iter>,
                                                CompileTimeErrorSignaler
                                            >
                                         >::type result;
                    };
                };
            };
        }
    }
}

#endif

