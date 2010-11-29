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

#ifndef _Attribute_h_
#define _Attribute_h_

#include <stdint.h>

#include "boost/mpl/integral_c.hpp"

#include "util/endianness.h"
#include "util/generic_endianess.h"

#include "object/PlacementNew.h"

#include "mw/attributes/access/Attribute_RT.h"

#include "mw/attributes/detail/AttributeSize.h"
#include "mw/attributes/detail/AttributeAsserts.h"
#include "mw/attributes/detail/ValueOffset.h"
#include "mw/attributes/detail/ValueByteCount.h"
#include "mw/attributes/detail/AttributeHeader.h"
#include "mw/attributes/detail/AttributeElementHeader.h"
#include "mw/attributes/tags/IntegralConstTag.h"
#include "mw/attributes/tags/AttributeTag.h"

#include "mw/attributes/detail/tags/TagSet.h"
#include "mw/attributes/detail/tags/IsHighDensity.h"
#include "mw/attributes/detail/tags/IsRequirable.h"
#include "mw/attributes/detail/tags/HasLessThanRelation.h"

#include "config/type_traits/ByteCount.h"

#include "assert/staticerror.h"

namespace famouso {
    namespace mw {
        namespace attributes {

            /*!
             * \brief Represents an attribute as a part of the %FAMOUSO generic attribute
             *  framework.
             *
             * This is the central entity of the attribute framework. It can be attached to
             *  disseminated events in order to be transmitted in a compact binary
             *  representation. Further %attributes can also be attached to event channels
             *  and network driver implementation to provide means for performing compile
             *  time composability checks. %Attribute attachment is generally done using
             *  type lists containing the specific attribute types.
             *
             * An attribute is defined by being either a high density or low density attribute
             *  and having a specific identifier. Related to this unique type an
             *  attribute has a specific value type describing the range of values it can
             *  have.
             *
             * An attribute class always has the value attached which was provided at compile
             *  time. This value is used when an instance of the specific class is created
             *  and the attribute is written to its binary representation. Nevertheless the
             *  runtime value, i.e. the value contained in the binary representation, can be
             *  accessed and, with limitations, even changed.
             *
             * The given base type is used for determining whether two given %attributes
             *  should be considered equal, that is of the same type. Since template classes
             *  with bound parameters always yield different types for different arguments,
             *  the plain type itself would not be considered as equal.
             *
             * \tparam BaseType The type which is used to compare %attributes of the same type
             * \tparam CompareTag The tag which defines how this attribute can be compared
             * \tparam ValueType The type of this attribute's value
             * \tparam Value The compile time value of this attribute
             * \tparam Comparator The filter expression struct defining how attributes
             *  of this type are compared, i.e. which are better than others
             * \tparam ID The identifier of this attribute
             * \tparam TagSet The tags attached to this attribute, an empty tag set
             *  by default
             */
            template <typename BaseType, typename CompareTag, typename ValueType,
                      ValueType Value, uint8_t ID, typename TagSet = detail::TagSet<> >
            class Attribute : public famouso::mw::attributes::access::Attribute_RT {
                public:
                    // The boost tag type, declaring the attribute class to be an
                    //  integral constant
                    typedef boost::mpl::integral_c_tag tag;

                    typedef tags::attribute_tag type_tag;
                    typedef CompareTag          compare_tag;
                    typedef BaseType            base_type;

                    // The type of the attribute's value
                    typedef ValueType value_type;

                    // The static value of this attribute
                    static const ValueType value = Value;

                    // The comparator filter expression for this attribute class, determined from
                    //  the given tag set
                    typedef typename
                        detail::HasLessThanRelation::template get_comparator<TagSet>::type comparator;

                    // The ID (aka category for system attributes) of this attribute
                    static const uint8_t id = ID;

                    // Determines whether this attribute is a high density attribute
                    static const bool highDensity =
                            TagSet::template contains_tag<detail::IsHighDensity>::value;

                    // Determines whether this attribute can occur in a requirement
                    static const bool requirable =
                            TagSet::template contains_tag<detail::IsRequirable>::value;

                    // A struct implementing the stronger relation between attributes
                    //  (generally delegates to less-than-or-equal respective
                    //   greater-than-or-equal relations)
                    template <typename OtherAttr>
                    struct isStronger :
                        public comparator::template apply_compiletime<Attribute, OtherAttr> {
                        FAMOUSO_STATIC_ASSERT_ERROR(
                            requirable,
                            only_requirable_attributes_may_have_a_stronger_relation,
                            (Attribute));
                    };

                    // The data array contains the binary representation of
                    //  this attribute (header + value)
                    uint8_t data[detail::AttributeSize<Attribute>::value];

                private:
                    // Static dummy typedefs
                    typedef typename detail::ValueTypeAssert<value_type>::dummy       valueTypeAssert;
                    typedef typename detail::IdBitCountAssert<highDensity, id>::dummy idBitCountAssert;

                protected:
                    Attribute() {
                        // Initialize the member array "data" to the binary representation
                        //  of this attribute

                        // The complex initialization (i.e. copying the bytes into the array)
                        //  only applies if the value takes at least one bit (e.g. 0 does not)
                        if (BitCount<value_type, value>::value > 0) {
                            // Get a big endian representation of the value
                            const ValueType bigEndian = famouso::util::hton(value);
                            // Get a pointer to the value
                            const uint8_t* ptr = reinterpret_cast<const uint8_t*> (&bigEndian);
                            // Move the pointer to the last byte
                            ptr += sizeof(ValueType) - 1;

                            // The index where we start writing the value (starts at the last
                            //  byte which will be written and will then be decremented)
                            uint8_t i = detail::AttributeSize<Attribute>::value-1;

                            // Copy as many bytes as either fit the range where the value is
                            //  supposed to be written or as the value itself has (as sizeof()
                            //  determines)
                            uint8_t j = detail::ValueByteCount<Attribute>::value;
                            do {
                                data[i--] = *ptr--;
                            } while (--j != 0);
                        } else {
                            // If the value's representation does not need any bit we simply zero
                            //  out the member array, the header will overwrite everything needed
                            //
                            //  1. System attribute, non-extended value
                            //      -here the whole attribute only consists of one
                            //       byte, so zeroing the "last" (the only) byte is
                            //       sufficient
                            //  2. Non-system attribute, non-extended length (length = 0 in this case)
                            //      -Nothing more has to be done here, since the
                            //       header will assign (not OR) itself to the first
                            //       two bytes (0xFFFF+length-part and the ID-part)
                            if (highDensity) {
                                  data[0] = 0x00;
                            }
                        }

                        // Write the header
                        // (It is essential to do that now, since the algorithm above does
                        //  not care about a possibly pre-written header, but the header
                        //  writer itself cares for a value possibly already written)
                        new (&data[0]) detail::AttributeHeader<Attribute> ;
                    }

                public:
                    const ValueType getValue() const {
                        return (Attribute_RT::getValue<ValueType>());
                    }

                    const bool setValue(const ValueType newValue) {
                        return (Attribute_RT::setValue<ValueType>(newValue));
                    }

            };

        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif // _Attribute_h_
