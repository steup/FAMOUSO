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

#ifndef _ByteCount_h_
#define _ByteCount_h_

#include <stdint.h>

#include "boost/mpl/eval_if.hpp"
#include "boost/mpl/int.hpp"
#include "boost/mpl/plus.hpp"


/*!
 * \brief Calculates the least number of bits needed to represent the
 *  given value of the specified type
 *
 * \tparam ValueType The type of the value
 * \tparam Value The value to calculate the bit count for
 */
template <typename ValueType, ValueType Value>
struct BitCount {
        /*!
         * \brief The value type as defined by Boost MPL
         */
        typedef ValueType value_type;
        /*!
         * \brief The tag type as defined by Boost MPL
         */
        typedef boost::mpl::integral_c_tag tag;
        /*!
         * \brief The self type as defined by Boost MPL
         */
        typedef BitCount type;

        /*!
         * \brief The value as defined by Boost MPL.
         *
         * The value represents the number of bits needed to represent the
         *  value given as the second template argument.
         */
        static const uint16_t value = boost::mpl::eval_if_c<(Value != 0),
                                                            boost::mpl::plus<BitCount<ValueType, (Value >> 1)>,
                                                                             boost::mpl::int_<1>
                                                                            >,
                                                            boost::mpl::int_<0>
                                                           >::type::value;
};

template <>
struct BitCount<bool, true> {
        /*!
         * \brief The value type as defined by Boost MPL
         */
        typedef bool value_type;
        /*!
         * \brief The tag type as defined by Boost MPL
         */
        typedef boost::mpl::integral_c_tag tag;
        /*!
         * \brief The self type as defined by Boost MPL
         */
        typedef BitCount type;

        static const uint16_t value = 1;
};

template <>
struct BitCount<bool, false> {
        /*!
         * \brief The value type as defined by Boost MPL
         */
        typedef bool value_type;
        /*!
         * \brief The tag type as defined by Boost MPL
         */
        typedef boost::mpl::integral_c_tag tag;
        /*!
         * \brief The self type as defined by Boost MPL
         */
        typedef BitCount type;

        static const uint16_t value = 0;
};

/*!
 * Macro to create template struct overloads for signed integral types.
 */
#define BitCountSigned(stype)                                               \
	template <stype Value>                                                  \
	struct BitCount<stype, Value> {                                         \
		                                                                    \
		typedef stype value_type;                                           \
		                                                                    \
		typedef boost::mpl::integral_c_tag tag;                             \
		                                                                    \
		typedef BitCount type;                                              \
		                                                                    \
		static const uint16_t value = (Value < 0) ?                         \
                                      (sizeof(stype) * 8) :                 \
                                      BitCount<                             \
                                        uint64_t,                           \
                                        static_cast<uint64_t>(Value)        \
                                      >::value;                             \
	}

BitCountSigned(int8_t);
BitCountSigned(int16_t);
BitCountSigned(int32_t);
BitCountSigned(int64_t);

#undef BitCountSigned

/*!
 * \brief Calculates the number of whole bytes needed to store the given
 *  value of the given type.
 *
 * If the last byte needed to represent the value has only several (less than
 *  eight) bits set, this byte is also considered in the result.
 *
 * \tparam ValueType The type of the value
 * \tparam Value The value to calculate the byte count of
 */
template <typename ValueType, ValueType Value>
struct ByteCount {
    private:
        static const uint16_t bitCount = BitCount<ValueType, Value>::value;

    public:
        /*!
         * \brief The result of the byte count calculation
         */
        static const uint16_t value = (bitCount % 8 == 0) ?
                                          (bitCount / 8) :
                                          ((bitCount / 8) + 1);
};

// Runtime bit / byte count calculation

/*!
 * \brief Performs the calculation of the bit count for the given
 *  64 bit unsigned value at runtime.
 *
 * \param value The value to calculate the bit count for
 *
 * \return The number of bits needed to represent the given value
 */
inline const uint16_t getBitCount(uint64_t value) {
    uint16_t result = 0;

    while (value > 0) {
        ++result;

        value >>= 1;
    }

    return (result);
}

/*!
 * \brief Performs the calculation of the bit count for the given
 *  8 bit unsigned value at runtime.
 *
 * \param value The value to calculate the bit count for
 *
 * \return The number of bits needed to represent the given value
 */
static inline const uint16_t getBitCount(uint8_t value) {
    return (getBitCount(static_cast<uint64_t>(value)));
}

/*!
 * \brief Performs the calculation of the bit count for the given
 *  16 bit unsigned value at runtime.
 *
 * \param value The value to calculate the bit count for
 *
 * \return The number of bits needed to represent the given value
 */
static inline const uint16_t getBitCount(uint16_t value) {
    return (getBitCount(static_cast<uint64_t>(value)));
}

/*!
 * \brief Performs the calculation of the bit count for the given
 *  32 bit unsigned value at runtime.
 *
 * \param value The value to calculate the bit count for
 *
 * \return The number of bits needed to represent the given value
 */
static inline const uint16_t getBitCount(uint32_t value) {
    return (getBitCount(static_cast<uint64_t>(value)));
}

/*!
 * \brief Performs the calculation of the bit count for the given
 *  boolean value at runtime.
 *
 * \param value The boolean value to calculate the bit count for
 *
 * \return The number of bits needed to represent the given value, that is
 *  either 1 for true or 0 for false
 */
static inline const uint16_t getBitCount(bool value) {
    return ((value) ? 1 : 0);
}

/*!
 * \brief Performs the calculation of the bit count for the given
 *  signed value at runtime.
 *
 * \param value The value to calculate the bit count for
 *
 * \return The number of bits needed to represent the given value
 */
static inline const uint16_t getBitCount(int8_t value) {
    return ((value < 0) ? sizeof(int8_t) : getBitCount(static_cast<uint64_t>(value)));
}
/*!
 * \brief Performs the calculation of the bit count for the given
 *  signed value at runtime.
 *
 * \param value The value to calculate the bit count for
 *
 * \return The number of bits needed to represent the given value
 */
static inline const uint16_t getBitCount(int16_t value) {
    return ((value < 0) ? sizeof(int16_t) : getBitCount(static_cast<uint64_t>(value)));
}
/*!
 * \brief Performs the calculation of the bit count for the given
 *  signed value at runtime.
 *
 * \param value The value to calculate the bit count for
 *
 * \return The number of bits needed to represent the given value
 */
static inline const uint16_t getBitCount(int32_t value) {
    return ((value < 0) ? sizeof(int32_t) : getBitCount(static_cast<uint64_t>(value)));
}
/*!
 * \brief Performs the calculation of the bit count for the given
 *  signed value at runtime.
 *
 * \param value The value to calculate the bit count for
 *
 * \return The number of bits needed to represent the given value
 */
static inline const uint16_t getBitCount(int64_t value) {
    return ((value < 0) ? sizeof(int64_t) : getBitCount(static_cast<uint64_t>(value)));
}

/*!
 * \brief Converts the given number of bits to whole bytes needed to
 *  store the specified number of bits at runtime.
 *
 * \param value The bit count to convert
 *
 * \return The number of whole bytes needed
 */
inline const uint16_t bitCountToByteCount(const uint16_t value) {
    // Similar to
    //  return (((value % 8) == 0) ? (value / 8) : ((value / 8) + 1));
    //
    return (((value & 0x7) == 0) ? (value >> 3) : ((value >> 3) + 1));
}

#endif
