/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Marcus Foerster <MarcusFoerster1@gmx.de>
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
#ifndef __AttributeDetailCompileErrors_h__
#define __AttributeDetailCompileErrors_h__

#include <stdint.h>

#include "assert/static.h"

#include "boost/mpl/int.hpp"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

                /*!
                 * \brief Struct used by the attribute sequence header to assert, that
                 *  the sequence size fits the binary format
                 *
                 * \tparam seqSize The sequence size
                 */
                template <uint16_t seqSize>
                struct ExtendedSequenceBoundError {
                        typedef ExtendedSequenceBoundError type;

                        // In case of an extension, assert that the sequence size does not
                        //  exceed the bounds of 15 bits (0x7FFF)
                        FAMOUSO_STATIC_ASSERT_ERROR(
                            ((seqSize & 0x7FFF) == seqSize),
                            sequence_length_exceeds_extended_format_bounds,
                            (boost::mpl::int_<seqSize>));
                };

                /*!
                 * \brief Struct used by the attribute sequence header to assert, that
                 *  the sequence size fits the binary format
                 *
                 * \tparam seqSize The sequence size
                 */
                template <uint16_t seqSize>
                struct UnextendedSequenceBoundError {
                        typedef UnextendedSequenceBoundError type;

                        // In case of no extension, assert that the sequence size does not
                        //  exceed the bounds of 7 bits (0x7F)
                        FAMOUSO_STATIC_ASSERT_ERROR(
                            ((seqSize & 0x7F) == seqSize),
                            sequence_length_exceeds_unextended_format_bounds,
                            (boost::mpl::int_<seqSize>));
                };

                /*!
                 * \brief Configurable error struct used for the static composability
                 *  checks.
                 *
                 */
                template <bool isRequirable, typename RelatedAttr, typename Requirement, bool compileError>
                struct AttributeIsNotRequireable {
                        typedef AttributeIsNotRequireable type;

                        FAMOUSO_STATIC_ASSERT(
                            isRequirable,
                            only_requirable_attributes_may_be_used_in_a_requirement,
                            (RelatedAttr, Requirement),
                            compileError);
                };

                /*!
                 * \brief Configurable error struct used for the static composability
                 *  checks.
                 *
                 */
                template <bool contained, typename RelatedAttr, typename Provision, bool compileError>
                struct RequiredAttributeNotContainedInProvision {
                        typedef RequiredAttributeNotContainedInProvision type;

                        FAMOUSO_STATIC_ASSERT(
                            contained,
                            required_attribute_not_contained_in_provision,
                            (RelatedAttr, Provision),
                            compileError);
                };

                /*!
                 * \brief Configurable error struct used for the static composability
                 *  checks.
                 *
                 */
                template <bool valueFits, typename RelatedAttr, typename ProvAttr, bool compileError>
                struct RequiredValueNotProvided {
                        typedef RequiredValueNotProvided type;

                        FAMOUSO_STATIC_ASSERT(
                            valueFits,
                            required_value_not_provided,
                            (RelatedAttr, ProvAttr),
                            compileError);
                };
            }
        }
    }
}
#endif

