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
#ifndef __AttributeFilterGrammarCompileErrors_h__
#define __AttributeFilterGrammarCompileErrors_h__

#include <boost/mpl/assert.hpp>
#include "mw/attributes/tags/AttributeTag.h"
#include "mw/attributes/tags/SameTag.h"
#include "mw/attributes/tags/CompareDifferentAttributesTag.h"

namespace famouso {
    namespace mw {
        namespace attributes {

            /*! \brief Generates a generic compile-time error.
             *
             *  There exists some specialisations of that class, describing the
             *  error more in detail and to guide the user to problem and maybe
             *  to a solution, to fix the problem.
             */
            template <typename T, typename U, typename V=int>
            struct CompileError {
                typedef CompileError    type;
                // this will be triggered if this type is instantiated
                BOOST_MPL_ASSERT_MSG(
                    sizeof(T) == 0,
                    generic_CompileError_from_attribute_framework,
                    ()
                );
            };

            /*! \brief is a specialization of %CompileError
             * \copydoc CompileError
             *  */
            template <typename T, typename U>
            struct CompileError<T, U, tags::same_tag> {
                typedef CompileError    type;
                // this will be triggered if this type is instantiated
                BOOST_MPL_ASSERT_MSG(
                    sizeof(T) == 0,
                    attribute_requires_comparison_with_attributes_of_the_same_type_only,
                    (T,U)
                );
            };

            /*! \brief is a specialization of %CompileError
             * \copydoc CompileError
             *  */
            template <typename T>
            struct CompileError<T, tags::attribute_tag> {
               typedef CompileError    type;
               // this will be triggered if this type is instantiated
               BOOST_MPL_ASSERT_MSG(
                   sizeof(T) == 0,
                   template_argument_meets_not_attribute_grammar_thus_it_seems_to_be_not_a_valid_attribute,
                   (T)
               );
            };

            /*! \brief is a specialization of %CompileError
             * \copydoc CompileError
             *  */
            template <typename T, typename U>
            struct CompileError<T, U, tags::compare_different_attributes_tag> {
                typedef CompileError    type;
                // this will be triggered if this type is instantiated
                BOOST_MPL_ASSERT_MSG(
                    sizeof(T) == 0,
                    comparing_different_attribute_types_are_not_allowed_by_the_attribute_grammar,
                    (T,U)
                );
            };

        }
    }
}
#endif

