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

#ifndef _Attribute_Asserts_
#define _Attribute_Asserts_

#include <stdint.h>

#include "boost/mpl/assert.hpp"

#include "config/type_traits/ByteCount.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

                /*!
                 * \brief Struct to statically assert that only the integral primitive types are
                 *  used in the attribute framework
                 *
                 * \tparam ValueType The type which should be asserted to be an integral primitive
                 */
                template <typename ValueType>
                struct ValueTypeAssert {
                        typedef uint8_t dummy;

                    private:
                        BOOST_MPL_ASSERT_MSG(false,
                                             only_primitive_integral_types_allowed,
                                             (ValueType));
                };
                template <> struct ValueTypeAssert<bool> {
                        typedef uint8_t dummy;
                };
                template <> struct ValueTypeAssert<uint8_t> {
                        typedef uint8_t dummy;
                };
                template <> struct ValueTypeAssert<int8_t> {
                        typedef uint8_t dummy;
                };
                template <> struct ValueTypeAssert<uint16_t> {
                        typedef uint8_t dummy;
                };
                template<> struct ValueTypeAssert<int16_t> {
                        typedef uint8_t dummy;
                };
                template <> struct ValueTypeAssert<uint32_t> {
                        typedef uint8_t dummy;
                };
                template <> struct ValueTypeAssert<int32_t> {
                        typedef uint8_t dummy;
                };
                template <> struct ValueTypeAssert<uint64_t> {
                        typedef uint8_t dummy;
                };
                template <> struct ValueTypeAssert<int64_t> {
                        typedef uint8_t dummy;
                };

                /*!
                 * \brief Struct to statically assert that an ID of a system attribute only has 4 bits.
                 *
                 * \tparam IsSystem True if it is a system attribute
                 * \tparam ID The identifier of the attribute
                 */
                template <bool IsSystem, uint8_t ID>
                struct IdBitCountAssert {
                        typedef uint8_t dummy;

                    private:
                        static const uint16_t bitCount = famouso::config::BitCount<uint8_t, ID>::value;

                        static const bool cond = ((!IsSystem) || (bitCount < 5));

                        BOOST_MPL_ASSERT_MSG(cond,
                                             system_attribute_ID_cannot_have_more_than_4_bits,
                                             (boost::mpl::int_<ID>, boost::mpl::int_<bitCount>));
                };
            }; // end namespace binrep
        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif // _Attribute_Asserts_
