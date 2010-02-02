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

#ifndef __RunTimeFilterExpressions_logical_not_operator_h__
#define __RunTimeFilterExpressions_logical_not_operator_h__

#include "mw/attributes/filter/logical_not.h"
#include "mw/attributes/filter/UnaryFilterExpression.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace filter {

                /*! \brief  the logical not (!) operator
                 *
                 *          It is used in lambda filter-expression, enabling
                 *          to get the inverse of an expression.
                 *
                 *  \tparam A is an expression type
                 */
                template< class A >
                static inline
                const UnaryFilterExpression<
                    UnaryFilterExpression<A>,
                    filter::logical_not<
                        UnaryFilterExpression<A>
                    >
                >
                operator ! (const UnaryFilterExpression<A>& a)
                {
                    typedef UnaryFilterExpression<A> ExprT;
                    return UnaryFilterExpression<
                                ExprT,
                                filter::logical_not<
                                    UnaryFilterExpression<A>
                                >
                           > (ExprT(a));
                }

            } /* filter */
        } /* attributes */
    } /* mw */
} /* famouso */

#endif
