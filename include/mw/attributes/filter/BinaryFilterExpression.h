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

#ifndef __BinaryFilterExpressions_h__
#define __BinaryFilterExpressions_h__

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace filter {


                /*! \brief  BinaryFilterExpression defines a binary %filter
                 *          lambda expression that is direct callable via
                 *          the overloaded function call operator.
                 *
                 *          It is a runtime construct used with overloaded
                 *          operators to form lambda filter-expressions.
                 *
                 *  \tparam L is the left operand of the expression
                 *  \tparam R is the right operand of the expression
                 *  \tparam OP is the performed operation (e.g. &&, ||, etc.)
                 */
                template < class L, class R, class OP >
                struct BinaryFilterExpression {
                    typedef BinaryFilterExpression type;
                    L l_;
                    R r_;

                    BinaryFilterExpression (const L& l, const R& r) :
                        l_ (l), r_ (r) { }

                    template< typename T>
                    bool operator () (const T& d) const {
                        return OP::apply (l_, d, r_);
                    }

                };

            } /* filter */
        } /* attributes */
    } /* mw */
} /* famouso */

#endif
