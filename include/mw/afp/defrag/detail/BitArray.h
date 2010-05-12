/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __BITARRAY_H_21D62F9B61AF9F__
#define __BITARRAY_H_21D62F9B61AF9F__

#include <stdint.h>
#include <string.h>
#include "mw/afp/shared/div_round_up.h"
#include "object/Allocator.h"
#include "mw/afp/Config.h"

namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {
                namespace detail {

                    /*!
                     *  \brief  Represents a bit inside a byte
                     *  \see    BitArray
                     */
                    class Bit {
                            uint8_t & byte;
                            uint8_t mask;
                        public:

                            /*!
                             *  \brief  Constructor
                             *  \param  byte    Byte which the bit is a part of
                             *  \param  mask    Mask defining the bit position
                             */
                            Bit(uint8_t & byte, uint8_t mask) :
                                    byte(byte), mask(mask) {
                            }

                            /// Set the bit (set to one)
                            void set() {
                                byte |= mask;
                            }

                            /// Clear the bit (set to zero)
                            void clear() {
                                byte &= ~mask;
                            }

                            /// Returns whether this bit is set
                            bool get() const {
                                return byte & mask;
                            }

                            /// Returns whether this bit is set
                            bool value() const {
                                return get();
                            }

                            /// Operator for assigning a boolean value to a Bit
                            Bit & operator=(bool value) {
                                if (value)
                                    set();
                                else
                                    clear();
                                return *this;
                            }
                    };


                    /*
                     *  \brief  BitArray of constant length
                     *  \tparam N   Number of bits (> 0)
                     *  \tparam Allocator   unused
                     */
                    template <size_t N, class Allocator = object::Allocator>
                    class BitArray {
                            enum { array_length = (N - 1) / 8 + 1 };
                            uint8_t array[array_length];
                        public:

                            /*!
                             *  \brief  Constructor
                             *  \param  init_value  If true, bits are initially set to one.
                             *                      Otherwise they are all cleared to zero.
                             *  \note   After construction you can access N bits immediatly.
                             */
                            BitArray(bool init_value = false) {
                                memset(array, init_value ? 0xff : 0, array_length);
                            }

                            /*!
                             *  \brief  Access a bit
                             *  \param  bit_idx Number of the Bit (ranging from zero to
                             *          N-1.
                             *  \note   bit_idx only checked by assert.
                             */
                            Bit operator[](size_t bit_idx) {
                                FAMOUSO_ASSERT(bit_idx < N);
                                return Bit(array[bit_idx / 8], 1 << (bit_idx % 8));
                            }

                            /*!
                             *  \brief  Access a bit
                             *  \param  bit_idx Number of the Bit (ranging from zero to
                             *          N-1.
                             *  \note   bit_idx only checked by assert.
                             */
                            const Bit operator[](size_t bit_idx) const {
                                return (*const_cast<BitArray *>(this))[bit_idx];
                            }

                            /// Return number of bits this array consists of (N)
                            size_t size() const {
                                return N;
                            }

                            /*!
                             *  \brief  Resize array (not supported for constant length BitArray)
                             *  \return Always returns false
                             */
                            bool resize(size_t count) {
                                return false;
                            }
                    };

                    /*
                     *  \brief  BitArray with dynamic length
                     *  \tparam Allocator   Allocator to use for array
                     */
                    template <class Allocator>
                    class BitArray<afp::dynamic, Allocator> {
                            size_t bit_count;
                            uint8_t * array;
                            bool init_value;
                        public:

                            /*!
                             *  \brief  Constructor
                             *  \param  init_value  If true, bits are initially set to one.
                             *                      Otherwise they are all cleared to zero.
                             *                      Also used to init bits on resize.
                             *  \note   After construction you have to call resize to reserve
                             *          memory for the array. Initially it does not hold
                             *          any bits.
                             */
                            BitArray(bool init_value = false) :
                                    bit_count(0), array(0), init_value(init_value) {
                            }

                            /*!
                             *  \brief  Access a bit
                             *  \param  bit_idx Number of the Bit (ranging from zero to
                             *          N-1.
                             *  \note   bit_idx only checked by assert.
                             */
                            Bit operator[](size_t bit_idx) {
                                FAMOUSO_ASSERT(bit_idx < bit_count);
                                return Bit(array[bit_idx / 8], 1 << (bit_idx % 8));
                            }

                            /*!
                             *  \brief  Access a bit
                             *  \param  bit_idx Number of the Bit (ranging from zero to
                             *          N-1.
                             *  \note   bit_idx only checked by assert.
                             */
                            const Bit operator[](size_t bit_idx) const {
                                return (*const_cast<BitArray *>(this))[bit_idx];
                            }

                            /// Return number of bits this array consists of
                            size_t size() const {
                                return bit_count;
                            }

                            /*!
                             *  \brief  Resize BitArray
                             *  \param  new_bit_count   New count of bits
                             *  \return Returns true on success.
                             */
                            bool resize(size_t new_bit_count) {
                                size_t old_len = shared::div_round_up(bit_count, 8);
                                size_t new_len = shared::div_round_up(new_bit_count, 8);

                                uint8_t * new_el = Allocator::alloc(new_len);
                                if (!new_el)
                                    return false;

                                uint8_t init_byte = init_value ? 0xff : 0;
                                if (array) {
                                    if (new_len > old_len) {
                                        memcpy(new_el, array, old_len);
                                        memset(new_el + old_len, init_byte, new_len - old_len);
                                    } else {
                                        memcpy(new_el, array, new_len);
                                        uint8_t last_byte_mask = new_bit_count % 8;
                                        if (last_byte_mask) {
                                            last_byte_mask = 0xff << (last_byte_mask);
                                            new_el[new_len-1] = (new_el[new_len-1] & ~last_byte_mask) | (init_byte & last_byte_mask);
                                        }
                                    }
                                    Allocator::free(array);
                                } else {
                                    memset(new_el, init_byte, new_len);
                                }

                                array = new_el;
                                bit_count = new_bit_count;

                                return true;
                            }
                    };


                } // namespace detail
            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __BITARRAY_H_21D62F9B61AF9F__

