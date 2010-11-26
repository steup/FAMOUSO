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

#ifndef __RelationalOperatorFilter_h__
#define __RelationalOperatorFilter_h__

#include "mw/attributes/filter/find.h"
#include "mw/attributes/tags/CompileTimeFilterTag.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace filter {


                /*! \brief  implements a relational operator %filter where the
                 *          operator function is configurable
                 *
                 *          It represents a binary %filter. But instead of
                 *          using the BinaryFilterExpression which would be
                 *          also possible, it is implemented with the knowledge
                 *          in mind that the first template parameter is used
                 *          only for search within events, thus it is not
                 *          needed to store it like it is in case of the usual
                 *          BinaryFilterExpression.
                 *
                 *  \tparam A is an attribute type
                 *  \tparam B is a type that should be comparable with A, and
                 *          it has to be stored here for later using it
                 *  \tparam op is the relational operator used
                 *  \tparam tag is used for selecting a special implementation
                 *          in case of using a compile-time %filter, where the
                 *          B's value is not a runtime entity but instead a
                 *          compile-time constant
                 */
                template< typename A, typename B, typename op, typename tag=void>
                struct RelationalOperatorFilter {
                    typedef RelationalOperatorFilter type;
                    B   __b;
                    /*! \brief  store the attribute B for later use
                     */
                    RelationalOperatorFilter (const B& t) : __b(t) {}

                    /*!\brief   the %filter acts as functor, thus its interface
                     *          supports calling like a usual function
                     *
                     * \tparam  t should be of type famouso::mw::Event
                     * \returns true or false dependent on the filter's result
                     */
                    template< typename T>
                    bool operator () (const T& e) const {
                        A* a=filter::find<A>(e);
                        return !!(a && op::apply_runtime( a->getValue(), __b.getValue() ) );
                    }
                };


                /*! \brief is a specialization of %RelationalOperatorFilter used for
                 *         resource-optimized %filter. Such filters, compile-time
                 *         %filter, can be often generated, when special conditions
                 *         for the %attributes are given.
                 *
                 * \copydoc RelationalOperatorFilter
                 */
                template< typename A, typename B, typename op>
                struct RelationalOperatorFilter< A, B, op, tags::compile_time_filter_tag > {
                    typedef RelationalOperatorFilter type;
                    RelationalOperatorFilter (const B&) {}

                    /*!\brief   the %filter acts as functor, thus its interface
                     *          supports calling like a usual function, but in case
                     *          of a compile-time %filter this is not possible. Hence,
                     *          for compile-time the apply function is supported.
                     *
                     * \tparam  t should be of type famouso::mw::Event
                     * \returns true or false dependent on the filter's result
                     */
                    template< typename T>
                    static bool apply(const T& e) {
                        A* a=filter::find<A>(e);
                        return !!(a && op::apply_runtime( a->getValue(), B::value ) );
                    }

                    /*!\brief   the %filter acts as functor, thus its interface
                     *          supports calling like a usual function
                     *
                     * \tparam  t should be of type famouso::mw::Event
                     * \returns true or false dependent on the filter's result
                     */
                    template< typename T>
                    bool operator () (const T& e) const {
                        return apply(e);
                    }
                };

            } /* filter */
        } /* attributes */
    } /* mw */
} /* famouso */

#endif

