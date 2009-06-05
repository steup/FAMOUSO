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

#ifndef __LocalCreatorTrait_h__
#define __LocalCreatorTrait_h__

/*! \brief LocalCreatorTrait is a trait template for the compile time selection
 *         utility IF_CONTAINS_TYPE_(NAME).
 *
 *         It is intended for the use within the then clause, because it
 *         creates a local %object of the requested type. A local %object is
 *         construced and when the scope of the function is left then the
 *         %object is destructed. This creator can be used for e.g. %device
 *         drivers initialisation or things that have to be done only once.
 *         However, it should be noted, that the destructor may not set back
 *         all things.
 
 *
 *  \tparam R is the return value of the static process method and its
 *          default is \c void
 *
 */
template <typename R=void>
struct LocalCreatorTrait {
    /*! \brief The method is provided for calling within the context of a
     *         selector template. As the name of the traits class describe it
     *         creates a local %object that is also destructed if the method is
     *         left.
     *
     *  \tparam R is the return value of the static process method and its
     *          default is \c void
     *  \tparam T is a type normally a configuration type/class
     *  \tparam SubType is a type and you can instantiate an
     *          object of it. If the trait is used in the context of
     *          IF_CONTAINS_TYPE_(NAME) it is guaranteed, that SubType is the
     *          requested type.
     *
     */
    template< typename T, typename SubType>
    static __attribute__((always_inline)) R process() {
        // here we give an additional attribute to still the compiler,
        // because we only want to create an %object and call its
        // constructor
        SubType _t __attribute__((unused));
        return R();
    }
};
#endif

