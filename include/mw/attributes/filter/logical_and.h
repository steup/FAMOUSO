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

#ifndef __logical_and_h__
#define __logical_and_h__

#include "mw/attributes/filter/logical_true.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace filter {

                /*!\brief   implements a logical and %filter
                 *
                 * \tparam  A is a %filter
                 * \tparam  B is a %filter
                 * \tparam  C is a %filter that default is
                 *          the logical_true %filter
                 * \tparam  D is a %filter that default is
                 *          the logical_true %filter
                 * \tparam  E is a %filter that default is
                 *          the logical_true %filter
                 */
                template< typename A,
                          typename B,
                          typename C=logical_true,
                          typename D=logical_true,
                          typename E=logical_true
                        >
                struct logical_and {

                    /*!\brief   the %filter function
                     *
                     * \tparam  t is the argument type of the function which
                     *          should be of type famouso::mw::Event
                     * \returns as result the logical 'and' combination of
                     *          the results of the applied filters
                     */
                    template< typename t>
                    static bool apply(const t &e) {
                        return A::apply(e) && B::apply(e) && C::apply(e) &&
                               D::apply(e) && E::apply(e);
                    }

                    template< typename T>
                    static bool apply(const A& a, const T &e, const B& b) {
                        return a(e) && b(e);
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

