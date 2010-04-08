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

#include "AttributeSize.h"
#include "AttributeAsserts.h"
#include "ValueOffset.h"
#include "ValueByteCount.h"
#include "AttributeHeader.h"
#include "AttributeElementHeader.h"
#include "ByteCount.h"
#include "generic_endianess.h"

namespace famouso {
	namespace mw {
		namespace attributes {

			/*!
			 * \brief Represents an attribute as a part of the FAMOUSO generic attribute
			 *  framework.
			 *
			 * This is the central entity of the attribute framework. It can be attached to
			 *  disseminated events in order to be transmitted in a compact binary
			 *  representation. Further attributes can also be attached to event channels
			 *  and network driver implementation to provide means for performing compile
			 *  time composability checks. Attribute attachment is generally done using
			 *  type lists containing the specific attribute types.
			 * An attribute is defined by being either a system or a non-system attribute
			 *  and having a specific identifier. Related to this unique type an
			 *  attribute has a specific value type describing the range of values it can
			 *  have.
			 * An attribute class always has the value attached which was provided at compile
			 *  time. This value is used when an instance of the specific class is created
			 *  and the attribute is written to its binary representation. Nevertheless the
			 *  runtime value, i.e. the value contained in the binary representation, can be
			 *  accessed and, with limitations, even changed.
			 *
			 * \tparam ValueType The type of this attribute's value
			 * \tparam Value The compile time value of this attribute
			 * \tparam ID The identifier of this attribute
			 * \tparam IsSystem True, if this is a system attribute
			 */
			template <typename ValueType, ValueType Value, uint8_t ID, bool IsSystem = false>
			class ExtendedAttribute {
				public:
					// The boost tag type, declaring the attribute class to be an integral constant
					typedef boost::mpl::integral_c_tag tag;

					// This type
					typedef ExtendedAttribute type;

					// The type of the attribute's value
					typedef ValueType value_type;

					// The static value of this attribute
					static const ValueType value = Value;

					// The ID (aka category for system attributes) of this attribute
					static const uint8_t id = ID;

					// Determines whether this attribute is a system attribute
					static const bool isSystem = IsSystem;

					// The data array contains the binary representation of this attribute (header + value)
					uint8_t data[AttributeSize<type>::value];

				private:
					// Static dummy instances
					static const ValueTypeAssert<value_type>    valueTypeAssert;
					static const IdBitCountAssert<isSystem, id> idBitCountAssert;

				public:
					ExtendedAttribute() {
						// Initialize the member array "data" to the binary representation
						//  of this attribute

						// Get a big endian representation of the value
						const ValueType bigEndian = famouso::util::hton(static_cast<ValueType>(value));
						// Get a pointer to the value
						const uint8_t*  ptr       = reinterpret_cast<const uint8_t*>(&bigEndian);
						// Move the pointer to the last byte
						ptr += sizeof(ValueType) - 1;

						// The index where we start writing the value (starts at the last byte which will be
						//  written and will then be decremented)
						uint16_t i = ValueOffset<type>::value + ValueByteCount<type>::value - 1;

						// Copy as many bytes as either fit the range where the value is supposed to be written or
						//  as the value itself has (as sizeof() determines)
						for (uint16_t j = 0; (i > ValueOffset<type>::value - 1) && (j < sizeof(ValueType)); --i, ++j) {
							data[i] = *ptr--;
						}

						// Write the header
						// (It is essential to do that now, since the algorithm above does not care for an eventually pre-written
						//  header, but the header writer itself cares for a value possibly already written)
						new (&data[0]) AttributeHeader<type>;
					}

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
						// The result (in big endian order first, it will be converted when it is returned)
						ValueType res = ValueType();

						// Pointer to the attribute value
						const uint8_t* val;

						const AttributeElementHeader* const header = reinterpret_cast<const AttributeElementHeader* const>(data);

						if ((isSystem) && (header->valueOrLengthSwitch)) {
							if (header->extension) {
								if (sizeof(ValueType) == 1) {
									// If the value type has only one byte, the two remaining bits of the
									//  header byte are zero, since all 8 bits of the value fit the extended byte
									res = static_cast<ValueType>(data[1]);
								} else {
									// In all other cases the 10 bits must be copied manually into the result

									// Get a pointer to the last byte of the result
									uint8_t* const tmp = reinterpret_cast<uint8_t* const>(&res) + sizeof(ValueType) - 1;

									// Copy the extended byte
									*tmp       = data[1];
									// Copy the remaining 2 bits
									*(tmp - 1) = data[0] & 0x3;
								}
							} else {
								res = static_cast<ValueType>(data[0] & 0x3);
							}
						} else {
							// Number of bytes to copy from the val-array
							uint16_t length;

							// Read the length in big-endian order
							uint8_t* const tmp = reinterpret_cast<uint8_t* const>(&length);
							if (header->extension) {
								tmp[0] = data[0] & (isSystem ? 0x3 : 0x7);
								tmp[1] = data[1];

								val    = &data[(isSystem ? 2 : 3)];
							} else {
								tmp[0] = 0x00;
								tmp[1] = data[0] & (isSystem ? 0x3 : 0x7);

								val    = &data[(isSystem ? 1 : 2)];
							}

							// Convert byte order of the length to host-order
							length = ntohs(length);

							// Interpret the result as a byte array to copy every single byte
							uint8_t* const resPtr = reinterpret_cast<uint8_t* const>(&res);

							// Copy the data from the determined pointer into the result
							for (uint16_t i = sizeof(ValueType) - 1, j = length - 1, counter = length; counter > 0; --i, --j, --counter) {
								resPtr[i] = val[j];
							}
						}

						return (famouso::util::ntoh(res));
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
						const uint16_t newBitCount = famouso::util::getBitCount(newValue);

						AttributeElementHeader* const header = reinterpret_cast<AttributeElementHeader* const>(data);

						if ((isSystem) && (header->valueOrLengthSwitch)) {
							if (header->extension) {
								if (newBitCount > 10) return (false);

								if (sizeof(ValueType) == 1) {
									// If the given value has only one byte, the 2 bits of the header must be
									//  cleared and the extended byte is set to the given value

									header->valueOrLength = 0x0;

									data[1] = static_cast<uint8_t>(newValue);
								} else {
									// This case can never be reached by a negative value (it would have taken 16 bits, which is not allowed here)

									// Convert the given value to big endian order
									const ValueType bigEndian = famouso::util::hton(newValue);

									// In all other cases the lower 10 bits of the given value must be copied
									//  into the binary representation
									// Retrieve a pointer and move it to the last byte of the value
									const uint8_t* const tmp = reinterpret_cast<const uint8_t* const>(&bigEndian) + sizeof(ValueType) - 1;

									// Copy into the extended byte
									data[1] =  *tmp;
									// Copy the lowest 2 bits of the header
									header->valueOrLength = (*(tmp - 1) & 0x3);
								}
							} else {
								if (newBitCount > 2) return (false);

								// This case can never be reached by a negative value (it takes 8 bits at least)
								//  so it is legal to simply mask the lowest 2 bits

								// Simply write the lowest 3 bits to the header
								header->valueOrLength = (newValue & 0x3);
							}
						} else {
							// Determine the byte count needed by the new value (this is necessary since
							//  we only write whole bytes if the VOL flag isn't set)
							const uint16_t newByteCount = famouso::util::bitCountToByteCount(newBitCount);

							// The pointer where to start copying the bytes of the new value (depends on the extension flag)
							uint8_t* targetPtr;
							// The length read
							uint16_t length;

							// Read the length in big-endian order
							uint8_t* const tmp = reinterpret_cast<uint8_t* const>(&length);
							if (header->extension) {
								tmp[0] = data[0] & (isSystem ? 0x3 : 0x7);
								tmp[1] = data[1];

								targetPtr = &data[(isSystem ? 2 : 3)];
							} else {
								tmp[0] = 0x00;
								tmp[1] = data[0] & (isSystem ? 0x3 : 0x7);

								targetPtr = &data[(isSystem ? 1 : 2)];
							}

							// Convert byte order of the length to host-order
							length = ntohs(length);

							// Check if the new byte count does not violate the old one
							if (newByteCount > length) return (false);

							// Convert the given value to big endian order
							const ValueType bigEndian = famouso::util::hton(newValue);
							// Get a pointer to the big endian representation
							const uint8_t* const newValPtr = reinterpret_cast<const uint8_t*>(&bigEndian) + (sizeof(ValueType) - length);

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
