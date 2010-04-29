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

#ifndef _EmptyAttribute_
#define _EmptyAttribute_

#include <stdint.h>

#include "boost/mpl/integral_c.hpp"

#include "util/endianness.h"
#include "util/generic_endianess.h"

#include "object/PlacementNew.h"

#include "mw/attributes/detail/AttributeSize.h"
#include "mw/attributes/detail/AttributeAsserts.h"
#include "mw/attributes/detail/ValueOffset.h"
#include "mw/attributes/detail/ValueByteCount.h"
#include "mw/attributes/detail/AttributeHeader.h"
#include "mw/attributes/detail/AttributeElementHeader.h"

#include "config/type_traits/ByteCount.h"

#include "mw/attributes/tags/IntegralConstTag.h"
#include "mw/attributes/tags/AttributeTag.h"

namespace famouso {
    namespace mw {
        namespace attributes {

            /*!
             * \brief Represents an attribute as a part of the FAMOUSO generic attribute
             *  framework.
             *
             * This is the central entity of the attribute framework. It can be attached to
             *  disseminated events in order to be transmitted in a compact binary
             *  representation. Further %attributes can also be attached to event channels
             *  and network driver implementation to provide means for performing compile
             *  time composability checks. %Attribute attachment is generally done using
             *  type lists containing the specific attribute types.
             *
             * An attribute is defined by being either a system or a non-system attribute
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
             *  should be considered as equal, that is of the same type. Since template classes
             *  with bound parameters always yield different types for different arguments,
             *  the plain type itself would not be considered as equal.
             *
             * \tparam BaseType The type which is used to compare attributes of the same type
             * \tparam CompareTag The tag which defines how this attribute can be compared
             * \tparam ValueType The type of this attribute's value
             * \tparam Value The compile time value of this attribute
             * \tparam ID The identifier of this attribute
             * \tparam IsSystem True, if this is a system attribute
             */
            template <typename BaseType, typename CompareTag, typename ValueType, ValueType Value, uint8_t ID, bool IsSystem = false>
            class Attribute {
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

                    // The ID (aka category for system attributes) of this attribute
                    static const uint8_t id = ID;

                    // Determines whether this attribute is a system attribute
                    static const bool isSystem = IsSystem;

                    // The data array contains the binary representation of
                    //  this attribute (header + value)
                    uint8_t data[detail::AttributeSize<Attribute>::value];

                private:
                    // Static dummy typedefs
                    typedef typename detail::ValueTypeAssert<value_type>::dummy    valueTypeAssert;
                    typedef typename detail::IdBitCountAssert<isSystem, id>::dummy idBitCountAssert;

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
                            uint16_t i = detail::ValueOffset<Attribute>::value + detail::ValueByteCount<Attribute>::value - 1;

                            // Copy as many bytes as either fit the range where the value is
                            //  supposed to be written or as the value itself has (as sizeof()
                            //  determines)
                            for (uint16_t j = 0; (i > detail::ValueOffset<Attribute>::value - 1) && (j < sizeof(ValueType)); --i, ++j) {
                                data[i] = *ptr--;
                            }
                        } else {
                            // If the value' representation does not need any bit we simply zero
                            //  out the member array, the header will overwrite everythinh needed
                            for (uint8_t i = 0; i < sizeof(data); ++i) {
                                data[i] = 0x00;
                            }
                        }

                        // Write the header
                        // (It is essential to do that now, since the algorithm above does
                        //  not care about an eventually pre-written header, but the header
                        //  writer itself cares for a value possibly already written)
                        new (&data[0]) detail::AttributeHeader<Attribute> ;
                    }

                public:
                    /*!
                     * \brief Returns the value from the binary representation of
                     *  this attribute instance.
                     *
                     * The value is parsed from the binary representation of this attribute.
                     *  That means this attribute's static properties are not used except for
                     *  the type and the is-system-flag.
                     *
                     * \return This attribute's value
                     */
                    const ValueType get() const {
                        // The result (in big endian order first, it will be converted when
                        //  it is returned)
                        ValueType res;

                        const detail::AttributeElementHeader* const header =
                                reinterpret_cast<const detail::AttributeElementHeader* const>(data);

                        if ((isSystem) && (header->valueOrLengthSwitch)) {
                            if (header->extension) {
                                if (sizeof(ValueType) == 1) {
                                    // If the value type has only one byte, the two remaining bits
                                    //  of the header byte are zero, since all 8 bits of the value
                                    //  fit the extended byte
                                    res = static_cast<ValueType>(data[1]);
                                } else {
                                    // In all other cases the 10 bits must be copied manually into
                                    //  the result

                                    // Assign the value directly to the result (The unnecessary
                                    //  header bits are still contained, they are masked after byte
                                    //  order conversion)
                                    res = *(reinterpret_cast<const ValueType*>(data));
                                    // Convert the value to host byte order and mask out the
                                    //  unnecessary header bits
                                    res = (famouso::util::ntoh(res) & 0x3FF);
                                }
                            } else {
                                res = static_cast<ValueType> (data[0] & 0x3);
                            }
                        } else {
                            // Initialize the result to zeros
                            res = ValueType();

                            // Pointer to the attribute value
                            const uint8_t* valPtr;

                            // Number of bytes to copy from the val-array
                            uint16_t length;

                            static const uint16_t lengthMask = (isSystem ? 0x3FF : 0x7FF);
                            static const uint8_t valueOffset = (isSystem ? 1 : 2);

                            // Read the length in big-endian order
                            if (header->extension) {
                                length = *(reinterpret_cast<const uint16_t*>(data));
                                length = (ntohs(length) & lengthMask);
                                valPtr = &data[valueOffset + 1];
                            } else {
                                // Expand the single byte to a 16 bit value and only apply the
                                //  high byte of the mask (0x3)
                                length = static_cast<uint16_t>(data[0] & 0x3);
                                valPtr = &data[valueOffset];
                            }

                            // Interpret the result as a byte array to copy every single byte
                            uint8_t* resPtr = reinterpret_cast<uint8_t* const>(&res) + sizeof(ValueType) - 1;
                            // Set the value pointer to the last byte of the value
                            valPtr += (length - 1);

                            while (length-- > 0) {
                                *resPtr-- = *valPtr--;
                            }

                            res = famouso::util::ntoh(res);
                        }

                        return (res);
                    }

                    /*!
                     * \brief Sets the value in the binary representation of
                     *  this attribute to the given one.
                     *
                     * Since the binary representation of attribute's value has
                     *  some restrictions in size it could be the case that it
                     *  is not possible to apply the given value. This could for
                     *  example be the case if the current value is written
                     *  unextended and the new value would demand to be written
                     *  extended.
                     *
                     * \param newValue The new value to set
                     *
                     * \return True if the value could properly be set, false
                     *  if the value could not be set and the attribute keeps
                     *  its previous value
                     */
                    const bool set(const ValueType newValue) {
                        // Determine the bit count of the value to set
                        const uint16_t newBitCount = getBitCount(newValue);

                        detail::AttributeElementHeader* const header =
                                reinterpret_cast<detail::AttributeElementHeader* const>(data);

                        if ((isSystem) && (header->valueOrLengthSwitch)) {
                            if (header->extension) {
                                if (newBitCount > 10)
                                    return (false);

                                if (sizeof(ValueType) == 1) {
                                    // If the given value has only one byte, the 2 bits of the header
                                    //  must be cleared and the extended byte is set to the given value

                                    header->valueOrLength = 0x0;

                                    data[1] = static_cast<uint8_t> (newValue);
                                } else {
                                    // This case can never be reached by a negative value (it would have
                                    //  taken 16 bits, which is not allowed here)

                                    // Convert the given value to big endian order
                                    const ValueType bigEndian = famouso::util::hton(newValue);

                                    // In all other cases the lower 10 bits of the given value must be
                                    //  copied into the binary representation
                                    // Retrieve a pointer and move it to the last byte of the value
                                    const uint8_t* const tmp = reinterpret_cast<const uint8_t* const>(&bigEndian) + sizeof(ValueType) - 1;

                                    // Copy the lowest 2 bits of the header
                                    header->valueOrLength = (*(tmp - 1) & 0x3);
                                    // Copy into the extended byte
                                    data[1] = *tmp;
                                }
                            } else {
                                if (newBitCount > 2)
                                    return (false);

                                // This case can never be reached by a negative value (it takes 8 bits at least)
                                //  so it is legal to simply mask the lowest 2 bits

                                // Simply write the lowest 3 bits to the header
                                header->valueOrLength = (newValue & 0x3);
                            }
                        } else {
                            // Determine the byte count needed by the new value (this is necessary since
                            //  we only write whole bytes if the VOL flag isn't set)
                            const uint16_t newByteCount = bitCountToByteCount(newBitCount);

                            // The pointer where to start copying the bytes of the new value
                            //  (depends on the extension flag)
                            uint8_t* targetPtr;
                            // The length read
                            uint16_t length;

                            static const uint16_t lengthMask  = (isSystem ? 0x3FF : 0x7FF);
                            static const uint8_t  valueOffset = (isSystem ? 1 : 2);

                            // Read the length in big-endian order
                            if (header->extension) {
                                length    = *(reinterpret_cast<const uint16_t*>(data));
                                length    = (ntohs(length) & lengthMask);
                                targetPtr = &data[valueOffset + 1];
                            } else {
                                // Expand the single byte to a 16 bit value and only apply the
                                //  high byte of the mask (0x3)
                                length    = static_cast<uint16_t>(data[0] & 0x3);
                                targetPtr = &data[valueOffset];
                            }

                            // Check if the new byte count does not violate the old one
                            if (newByteCount > length) return (false);

                            // Convert the given value to big endian order
                            const ValueType bigEndian = famouso::util::hton(newValue);
                            // Get a pointer to the big endian representation
                            const uint8_t* const newValPtr =
                                    reinterpret_cast<const uint8_t*>(&bigEndian) + (sizeof(ValueType) - length);

                            // Copy the value (we even copy the possible zero bytes of the new value to
                            //  zero out the bytes not needed, the length must not be decreased since
                            //	it would make iterating over the byte sequence impossible)
                            for (uint16_t i = 0; i < length; ++i) {
                                targetPtr[i] = newValPtr[i];
                            }
                        }

                        return (true);
                    }
            };

        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif // _EmptyAttribute_
