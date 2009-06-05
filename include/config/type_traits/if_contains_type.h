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
#ifndef __if_contains_type_h__
#define __if_contains_type_h__

#include "config/traits/EmptyTrait.h"
#include "config/type_traits/contains_type.h"

/*! \brief The IF_CONTAINS_TYPE_(NAME) macro generates a compile time selector
 *         class that allows for cheching on a specific subtype of type/class
 *         and enables code execution facilities in dependency of the existence
 *         of a subtype.
 *
 *  \param[in] NAME is the name of the subtype, that has to be tested.
 *
 *         The generated class if_contains_type_\a NAME supports first the type
 *         checking with the help of the macro CONTAINS_TYPE_(NAME) and second,
 *         it allows code execution in dependency of the existence of the
 *         requested type. For that purpose a compile time selector is
 *         integrated which itself is configurable with the action to support
 *         in case of existence or lack of the type. The default behaviour for
 *         else clause is do nothing, however it is up to the user to change
 *         that in a way that the user needs by providing a trait class. For an
 *         example, on how to provide a trait template class with the correct
 *         interface see ThenTraitExample or ElseTraitExample.
 *
 *         How to use it? Here is an example:
 *
 *         \code
 *
 *              struct TypeWithTestType {
 *                  typedef int TestType;
 *              };
 *
 *              IF_CONTAINS_TYPE_(TestType);
 *
 *              ...
 *
 *              // return 0, because int does not contain TestType
 *              if_contains_type_TestType<int>::value;
 *              // return 1, because TypeWithTestType contains TestType
 *              if_contains_type_TestType<TypeWithTestType>:: value;
 *
 *              // does nothing due to the fact that int does not contain
 *              // the type TestType and it executes nothing, because we
 *              // have specified an empty else clause
 *              if_contains_type_TestType<int>::ThenElse<Then>::process();
 *
 *              // executes the Else trait template due to the fact that
 *              // int does not contain the type TestType
 *              if_contains_type_TestType<int>::ThenElse<Then, Else>::process();
 *
 *              // executes the Then trait, because the requested type is contained
 *              if_contains_type_TestType<TypeWithTestType>::ThenElse<Then>::process();
 *
 *              ...
 *        \endcode
 *
 *  \author Michael Schulze
 *
 */
#define IF_CONTAINS_TYPE_(NAME)                                             \
template < typename T, typename R = void>                                   \
class if_contains_type_##NAME {                                             \
        CONTAINS_TYPE_(NAME);                                               \
        /* Generates Comile-Time Error if used in an ElseTrait          */  \
        struct NAME;                                                        \
                                                                            \
        /* a compile time selector for the traits the first is          */  \
        /* the general purpose template and the second is a             */  \
        /* specialization that is used if the subtype exists            */  \
        template<bool U, typename ThenTrait,                                \
                 typename ElseTrait>                                        \
        struct TraitSelect_##NAME {                                         \
            static inline R do_it() {                                       \
                return ElseTrait::template process<T, NAME>();              \
            }                                                               \
        };                                                                  \
                                                                            \
        template<typename ThenTrait, typename ElseTrait>                    \
        struct TraitSelect_##NAME<true, ThenTrait, ElseTrait> {             \
            static inline R do_it() {                                       \
                return ThenTrait::template process<T, typename T::NAME>();  \
            }                                                               \
        };                                                                  \
                                                                            \
     public:                                                                \
        /* the value signalise if the subtype is defined or not         */  \
        static const bool value = contains_type_##NAME<T>::value;           \
                                                                            \
        /* processes dependent on the value the respective template     */  \
        /* which leads to generating code                               */  \
        template <template <typename> class ThenTrait,                      \
                  template <typename> class ElseTrait=EmptyTrait >          \
        struct ThenElse {                                                   \
            static inline R process() {                                     \
                return TraitSelect_##NAME<                                  \
                            value,                                          \
                            ThenTrait<R>,                                   \
                            ElseTrait<R>                                    \
                       >::do_it();                                          \
            }                                                               \
        };                                                                  \
}

#endif

