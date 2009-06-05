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

#ifndef __ElseTraitExample_h__
#define __ElseTraitExample_h__

#include <iostream>

/*! \brief ElseTraitExample is a trait template for the compile time selection
 *         utility IF_CONTAINS_TYPE_(NAME).
 *
 *         It is an example on how to write such a trait, that can be used in
 *         the metioned context. It is intended to be used for the else path of
 *         the compile time switch, although the ThenTrait as well as the
 *         ElseTrait have the same interface.
 *
 *  \tparam R is the return value of the static process method and its
 *          default is \c void
 *
 */
template <typename R=void>
struct ElseTraitExample {
    /*! \brief The method is provided for calling within the context of a
     *         selector template. As the name of the traits class describe this
     *         is intended to be called within the else clause of the selector
     *         trait. This example outputs the function signature and it can
     *         be used as starting point for developing own user-specific
     *         ElseTraits.
     *
     *  \tparam R is the return value of the static process method and its
     *          default is \c void
     *  \tparam T is a type normally a configuration type/class
     *  \tparam SubTypeNotDefined is a type and you can not instantiate an
     *          object of it. Trying instantiation, leads to a compile time
     *          error if the trait is used in the context of
     *          IF_CONTAINS_TYPE_(NAME)
     *
     */
    template< typename T, typename SubTypeNotDefined>
    static __attribute__((always_inline)) R process() {
        std::cout << __PRETTY_FUNCTION__  << std::endl;
        return R();
    }
};

#endif
