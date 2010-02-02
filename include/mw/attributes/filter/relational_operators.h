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

#ifndef __RunTimeFilterExpressions_relational_operators_h__
#define __RunTimeFilterExpressions_relational_operators_h__

#include "mw/attributes/filter/greater_than.h"
#include "mw/attributes/filter/greater_than_or_equal_to.h"
#include "mw/attributes/filter/less_than.h"
#include "mw/attributes/filter/less_than_or_equal_to.h"
#include "mw/attributes/filter/equal_to.h"
#include "mw/attributes/filter/not_equal_to.h"
#include "mw/attributes/filter/logical_or.h"
#include "mw/attributes/filter/logical_and.h"
#include "mw/attributes/filter/logical_not.h"
#include "mw/attributes/filter/BinaryFilterExpression.h"
#include "mw/attributes/filter/UnaryFilterExpression.h"

#include "mw/attributes/filter/RelationalOperatorFilterReturnTypeCalculator.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace filter {

/*! \brief  Macro for the generation of relational operators. The generated
 *          operators can be used in lambda filter-expression.
 */
#define OPERATOR(op, op_class, p1, p2)                                      \
    template< class A, class B >                                            \
    static inline                                                           \
    const UnaryFilterExpression< typename                                   \
        filter::RelationalOperatorFilterReturnTypeCalculator<               \
            A,                                                              \
            B,                                                              \
            op_class                                                        \
        >::type                                                             \
    >                                                                       \
    operator op (const p1, const p2) {                                      \
        typedef typename                                                    \
            filter::RelationalOperatorFilterReturnTypeCalculator<           \
                A,                                                          \
                B,                                                          \
                op_class                                                    \
            >::type ReturnType;                                             \
        return UnaryFilterExpression< ReturnType > (ReturnType(b));         \
    }

OPERATOR( >=, filter::greater_than_or_equal_to, UnaryFilterExpression<A>& a, B& b )
OPERATOR( <=, filter::less_than_or_equal_to,    UnaryFilterExpression<A>& a, B& b )
OPERATOR( > , filter::greater_than,             UnaryFilterExpression<A>& a, B& b )
OPERATOR( < , filter::less_than,                UnaryFilterExpression<A>& a, B& b )
OPERATOR( !=, filter::not_equal_to,             UnaryFilterExpression<A>& a, B& b )
OPERATOR( ==, filter::equal_to,                 UnaryFilterExpression<A>& a, B& b )

OPERATOR( >=, filter::less_than_or_equal_to,    B& b, UnaryFilterExpression<A>& a )
OPERATOR( <=, filter::greater_than_or_equal_to, B& b, UnaryFilterExpression<A>& a )
OPERATOR( > , filter::less_than,                B& b, UnaryFilterExpression<A>& a )
OPERATOR( < , filter::greater_than,             B& b, UnaryFilterExpression<A>& a )
OPERATOR( !=, filter::not_equal_to,             B& b, UnaryFilterExpression<A>& a )
OPERATOR( ==, filter::equal_to,                 B& b, UnaryFilterExpression<A>& a )

#undef OPERATOR

            } /* filter */
        } /* attributes */
    } /* mw */
} /* famouso */

#endif
