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
 * $Id:
 *
 ******************************************************************************/

#ifndef _EmptyAttribute_
#define _EmptyAttribute_

#include <stdint.h>
#include <string>

#include "debug.h"

#include "boost/mpl/assert.hpp"

#include "util/endianness.h"

#include "AttrHeader.h"

/*!
 * Calculates the overall number of bytes used by the binary representation
 *  of the given attribute, that is, the header and the value.
 *
 * /tparam Attr The attribute type of which the overall size should be
 *  calculated
 */
template <typename Attr>
struct SizeCalculator {
	private:
		static const uint16_t byteCount = ValueByteCount<Attr>::value;

	public:
		/*!
		 * The calculated overall attribute size in bytes
		 */
		static const uint16_t value = CaseSelector<Attr, uint16_t, 1, 2, (1 + byteCount), (2 + byteCount), (1 + 1 + byteCount), (2 + 1 + byteCount)>::value;
};

template <typename Attr>
struct ValueOffsetCalculator {
	private:
		// For the second case (value fits extended) it is necessary to check if
		//  the value only needs one byte, which would mean that it is simply written
		//  into the second byte
		static const uint8_t res2 = (ValueBitCount<Attr>::value < 9) ? 1 : 0;

	public:
		static const uint8_t value = CaseSelector<Attr, uint8_t, 0, res2, 1, 2, 2, 3>::value;
};

template <typename ValueType>
struct ValueTypeAssert {
		BOOST_MPL_ASSERT_MSG(false, only_primitive_integral_types_allowed, (ValueType));
};
template <> struct ValueTypeAssert<bool>     {};
template <> struct ValueTypeAssert<uint8_t>  {};
template <> struct ValueTypeAssert<int8_t>   {};
template <> struct ValueTypeAssert<uint16_t> {};
template <> struct ValueTypeAssert<int16_t>  {};
template <> struct ValueTypeAssert<uint32_t> {};
template <> struct ValueTypeAssert<int32_t>  {};
template <> struct ValueTypeAssert<uint64_t> {};
template <> struct ValueTypeAssert<int64_t>  {};

// Generic host to network conversion

template <typename ValType>
inline const ValType hton(const ValType& value);

template <> inline const uint8_t hton<uint8_t>(const uint8_t& value) {
	return (value);
}
template <> inline const int8_t hton<int8_t>(const int8_t& value) {
	return (value);
}
template <> inline const uint16_t hton<uint16_t>(const uint16_t& value) {
	return (htons(value));
}
template <> inline const int16_t hton<int16_t>(const int16_t& value) {
	return (hton(reinterpret_cast<const uint16_t&>(value)));
}
template <> inline const uint32_t hton<uint32_t>(const uint32_t& value) {
	return (htonl(value));
}
template <> inline const int32_t hton<int32_t>(const int32_t& value) {
	return (hton(reinterpret_cast<const uint32_t&>(value)));
}
template <> inline const uint64_t hton<uint64_t>(const uint64_t& value) {
	return (htonll(value));
}
template <> inline const int64_t hton<int64_t>(const int64_t& value) {
	return (hton(reinterpret_cast<const uint64_t&>(value)));
}

// Generic network to host conversion

template <typename ValType>
inline const ValType ntoh(const ValType& value);

template <> inline const uint8_t ntoh<uint8_t>(const uint8_t& value) {
	return (value);
}
template <> inline const int8_t ntoh<int8_t>(const int8_t& value) {
	return (value);
}
template <> inline const uint16_t ntoh<uint16_t>(const uint16_t& value) {
	return (ntohs(value));
}
template <> inline const int16_t ntoh<int16_t>(const int16_t& value) {
	return (ntoh(reinterpret_cast<const uint16_t&>(value)));
}
template <> inline const uint32_t ntoh<uint32_t>(const uint32_t& value) {
	return (ntohl(value));
}
template <> inline const int32_t ntoh<int32_t>(const int32_t& value) {
	return (ntoh(reinterpret_cast<const uint32_t&>(value)));
}
template <> inline const uint64_t ntoh<uint64_t>(const uint64_t& value) {
	return (ntohll(value));
}
template <> inline const int64_t ntoh<int64_t>(const int64_t& value) {
	return (ntoh(reinterpret_cast<const uint64_t&>(value)));
}

template <typename ValueType, ValueType Value, uint8_t ID, bool IsSystem = false>
class ExtendedAttribute {
	public:
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
		uint8_t data[SizeCalculator<type>::value];

	private:
		// Dummy instance to assert that only primitive integral types will be used as value type
		ValueTypeAssert<ValueType> valueTypeAssert;

	public:
		ExtendedAttribute() {
			// Initialize the member array "data" to the binary representation
			//  of this attribute

			const ValueType bigEndian = hton(value);
			const uint8_t*  ptr       = reinterpret_cast<const uint8_t*>(&bigEndian);

			TO_HEX;

			DBG_MSG("Calculated size: " << ValueByteCount<type>::value);
			DBG_MSG("Actual size:     " << sizeof(ValueType));

			// Move the pointer to the last byte
			ptr += sizeof(ValueType) - 1;

			uint16_t i = ValueOffsetCalculator<type>::value + ValueByteCount<type>::value - 1;

			for (uint16_t j = 0; (i > ValueOffsetCalculator<type>::value - 1) && (j < sizeof(ValueType)); --i, ++j) {
				data[i] = *ptr--;
			}

			// Write the header
			new (&data[0]) AttributeHeaderWriter<type>;

			print(data, sizeof(data));
		}

		/*!
		 * /brief Returns the value from the binary representation of
		 *  this attribute instance.
		 *
		 * The value is parsed from the binary representation of this attribute.
		 *  That means this attribute's static properties are not used except for
		 *  the type and the is-system-flag.
		 *
		 * /return This attribute's value
		 */
		const ValueType get() const {
			// The result
			ValueType res = ValueType();

			// Pointer to the attribute value
			const uint8_t* val;
			// Number of bytes to copy from the val-array
			uint16_t length;

			const AttributeElementHeader* const header = reinterpret_cast<const AttributeElementHeader* const>(data);

			if (isSystem) {
				if (header->valueOrLengthSwitch) {
					if (header->extension) {
						if (sizeof(ValueType) == 2) {
							// If the value type has two bytes, we have to copy them manually
							uint8_t* const tmp = reinterpret_cast<uint8_t* const>(&res);

							tmp[0] = data[0] & 0x3;
							tmp[1] = data[1];
						} else {
							// TODO: Remove this again
							assert(sizeof(ValueType) == 1);

							// If the value type has only one byte, the two remaining bits of the
							//  header byte are zero, since all 8 bits of the value fit the extended byte
							res = static_cast<ValueType>(data[1]);
						}
					} else {
						res = static_cast<ValueType>(data[0] & 0x3);
					}

					// Everything is already done here, so we can (must) return
					return (ntoh(res));
				} else {
					// Read the length in big-endian order
					uint8_t* const tmp = reinterpret_cast<uint8_t* const>(&length);
					if (header->extension) {
						tmp[0] = data[0] & 0x3;
						tmp[1] = data[1];

						val    = &data[2];
					} else {
						tmp[0] = 0x00;
						tmp[1] = data[0] & 0x3;

						val    = &data[1];
					}

					// Convert byte order of the length to host-order
					length = ntohs(length);
				}
			} else {
				// Read the length in big-endian order
				uint8_t* const tmp = reinterpret_cast<uint8_t* const>(&length);
				if (header->extension) {
					tmp[0] = data[0] & 0x7;
					tmp[1] = data[1];

					val = &data[3];
				} else {
					tmp[0] = 0x00;
					tmp[1] = data[0] & 0x7;

					val = &data[2];
				}

				// The val pointer just skipped one byte, since we are not interested in
				//  the ID at this point

				// Convert byte order of the length to host-order
				length = ntohs(length);
			}

			::logging::log::emit() << "Starting to copy " << length << " bytes..." << ::logging::log::endl;

			// Interpret the result as a byte array to copy every single byte
			uint8_t* const resPtr = reinterpret_cast<uint8_t* const>(&res);

			// Copy the data from the determined pointer into the result
			for (uint16_t i = sizeof(ValueType) - 1, j = length - 1, counter = length; counter > 0; --i, --j, --counter) {
				resPtr[i] = val[j];
			}

			return (ntoh(res));
		}

		/*!
		 * /brief Sets the value in the binary representation of
		 *  this attribute to the given one.
		 *
		 * Since the binary representation of attribute's value has
		 *  some restrictions in size it could be the case that it
		 *  is not possible to apply the given value. This could for
		 *  example be the case if the current value is written
		 *  unextended and the new value would demand to be written
		 *  extended.
		 *
		 * /return True if the value could properly be set, false
		 *  if the value could not be set and the attribute keeps
		 *  its previous value
		 */
		const bool set(const ValueType newValue) {
			// Determine the bit count of the value to set
			const uint16_t newBitCount = getBitCount(newValue);

			AttributeElementHeader* header = (AttributeElementHeader*) data;

			// TODO Consider endianess

			if (isSystem) {
				if (header->valueOrLength) {
					if (header->extension) {
						if (newBitCount > 10) {
							return (false);
						}

						// Now the new bit count is 10 at max, so we copy the
						//  lower ten bits of the given value to the binary representation
						// (For signed values this means, that they are positive and can be
						//  safely cast to a 16 bit value)

						// Cast the new value to a 16 bit unsigned value and convert it to network order
						const uint16_t tmpValue = htons(static_cast<uint16_t>(&newValue));
						// Get a pointer to the value
						const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&tmpValue);

						// Write the highest 2 bits into the first byte
						header->valueOrLength = (ptr[0] & 0x3);
						// And the rest to the second byte
						data[1] = ptr[1];
					} else {
						if (newBitCount > 2) {
							return (false);
						}

						// Simply write the lowest 3 bits to the header
						header->valueOrLength = (newValue & 0x3);
					}
				} else {
					// The pointer where to start copying the bytes of the new value
					uint8_t* targetPtr;

					uint16_t length;

					// Read the length in big-endian order
					uint8_t* const tmp = reinterpret_cast<uint8_t* const>(&length);
					if (header->extension) {
						tmp[0] = data[0] & 0x3;
						tmp[1] = data[1];

						targetPtr = &data[2];
					} else {
						tmp[0] = 0x00;
						tmp[1] = data[0] & 0x3;

						targetPtr = &data[1];
					}

					// Convert byte order of the length to host-order
					length = ntohs(length);

					// Determine the byte count needed by the new value (this is necessary since
					//  we only write whole bytes if the VOL flag isn't set)
					const uint16_t newByteCount = bitCountToByteCount(newBitCount);

					// Check if the new byte count does not violate the old one
					if (newByteCount > length) {
						return (false);
					}

					// Convert
					const ValueType bigEndian = hton(newValue);

					// Copy the lowest bytes

				}
			} else {
				uint16_t length = header->length;
				uint8_t* ptr    = &data[2];

				if (header->extension) {
					length <<= 8;
					length |=  data[1];
					++ptr;
				}

				const uint16_t byteCount = bitCountToByteCount(newBitCount);

				if (byteCount > length) {
					return (false);
				}

				// TODO: Same here...

				uint64_t tmp = newValue;
				tmp          = htonll(tmp);

				for (uint16_t i = 0; i < length; ++i) {
					ptr[i] = ((uint8_t*) &tmp)[i];
				}
			}

			return (true);
		}
};

#endif // _EmptyAttribute_
