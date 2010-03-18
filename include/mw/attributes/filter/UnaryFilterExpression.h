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

#ifndef __UnaryFilterExpressions_h__
#define __UnaryFilterExpressions_h__

#include "mw/attributes/filter/logical_not.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace filter {


                /*! \brief  UnaryFilterExpression defines an unary lambda
                 *          filter-expression that is directly callable via
                 *          the overloaded function call operator.
                 *
                 *          It is a runtime construct. It is used to wrap
                 *          template filter-constructs, allowing to call this
                 *          via usual function call syntax. Furthermore, it
                 *          is used with overloaded  operators to form lambda
                 *          filter-expressions.
                 *
                 *  \tparam E is the expression that needs to be wraped
                 *  \tparam A is for tag types to allow easy specialisations.
                 */
                template < class E, typename A=void>
                struct UnaryFilterExpression {
                    typedef UnaryFilterExpression type;
                    E expr_;

                    UnaryFilterExpression (const E& e) : expr_ (e) {}
                    explicit UnaryFilterExpression () {}

                    template< typename T>
                    bool operator() (const T& d) const {
                        return expr_(d);
                    }

                };

                /*! \brief  specialisation of UnaryFilterExpression
                 *
                 *  \copydoc UnaryFilterExpression
                 *
                 *          The default argument for the second template
                 *          parameter is the filter::logical_not class
                 *          template. This specialization is used for
                 *          the not operator.
                 *
                 *  \tparam E is the expression that needs to be wraped
                 */
                template < class E>
                struct UnaryFilterExpression<E, filter::logical_not<E> > {
                    typedef UnaryFilterExpression type;
                    E expr_;

                    UnaryFilterExpression (const E& e) : expr_ (e) {}
                    explicit UnaryFilterExpression () {}

                    template< typename T>
                    bool operator() (const T& d) const {
                        return filter::logical_not<E>::apply(expr_, d);
                    }

                };

            } /* filter */
        } /* attributes */
    } /* mw */
} /* famouso */

#endif
