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

#ifndef __RAWSTORAGE_H_8AC69FC602CD2C__
#define __RAWSTORAGE_H_8AC69FC602CD2C__


#include <stdint.h>

#include "debug.h"
#include "config/type_traits/SmallestUnsignedTypeSelector.h"


namespace object {

    /*!
     * \brief %Storage for raw data blocks.
     *
     * Uses in place linked list for allocated/free blocks management. That's why
     * it's not possible to allocate \p size Bytes. But management information is
     * kept as small as possible by using the smallest possible integer type for
     * internal addressing.
     *
     * Over time the storage may suffer from memory fragmentation.
     *
     * \tparam size Number of bytes to use for storage.
     *
     * \par Logging:
     *
     * - Define RAWSTORAGE_LOG_USAGE before including RawStorage.h for %logging
     *   alloc() and free() calls on RawStorage objects.
     * - Define RAWSTORAGE_FULL_MEM_INFO to log complete RawStorage status on
     *   alloc() and free() calls.
     *
     * \author Philipp Werner
     */
    template <uint64_t size>
    class RawStorage {
        public:
            /// Type used for addressing inside storage (smallest for given size)
            typedef typename SmallestUnsignedTypeSelector<size>::type SizeT;

        protected:
            /// Buffer for memory to allocate and memory management information (AllocInfo structs)
            uint8_t data [size];

            /// Offset in data where we find first AllocInfo instance (if alloc_count != size)
            SizeT first_allocated;

            /// Memory management information. In front of each allocated block.
            struct AllocInfo {
                /// Length of the allocated block
                SizeT block_length;

                /// Offset in data where we find next AllocInfo instance (size if this is last)
                SizeT next_allocated;
            };

            /// Return AllocInfo pointer from offset in data member
            AllocInfo * get_ai(SizeT offset) {
                return reinterpret_cast<AllocInfo *>(data + offset);
            }

            /*!
             * \brief Find free memory for allocation via first fit algorithm
             * \param bytes [in] Number of bytes to allocate
             * \param to_allocate [out] Offset in data to allocate (AllocInfo)
             * \return Pointer to next_allocated offset of previous allocated block
             *         or NULL if there is no empty block that fits the requirements.
             */
            SizeT * find_first_fit(SizeT bytes, SizeT & to_allocate) {
                if (!bytes || bytes > size - sizeof(AllocInfo))
                    return 0;

                if (bytes + sizeof(AllocInfo) <= first_allocated) {
                    // Allocation at the beginning
                    to_allocate = 0;
                    return &first_allocated;
                } else {
                    // Allocation behind allocated blocks

                    // Add AllocInfo sizes only once (not in each comparison)
                    bytes += 2 * sizeof(AllocInfo);

                    // Check empty blocks between allocated ones
                    SizeT curr_ai = first_allocated;
                    AllocInfo * ai = get_ai(curr_ai);
                    while (curr_ai < size) {

                        if (curr_ai + ai->block_length + bytes <= ai->next_allocated) {
                            to_allocate = curr_ai + sizeof(AllocInfo) + ai->block_length;
                            return &ai->next_allocated;
                        }

                        curr_ai = ai->next_allocated;
                        ai = get_ai(curr_ai);
                    }

                    // If this assert fails there is a bug in this code or a buffer overflow
                    // in the code using the memory.
                    FAMOUSO_ASSERT(curr_ai == size);

                    // No block fits
                    return 0;
                }
            }

            /*!
             * \brief Find allocated memory for freeing
             * \param p [in] Pointer to allocated block
             * \return Pointer to next_allocated offset of previous allocated block
             *         or NULL if there is no allocated block that fits the requirements.
             */
            SizeT * find_allocated_block(uint8_t * p) {
                p -= sizeof(AllocInfo);

                SizeT * prev_ai_na_p = &first_allocated;
                AllocInfo * ai = get_ai(first_allocated);
                while (*prev_ai_na_p < size) {

                    if (ai == reinterpret_cast<AllocInfo *>(p))
                        return prev_ai_na_p;

                    prev_ai_na_p = &ai->next_allocated;
                    ai = get_ai(ai->next_allocated);
                }

                // If this assert fails there is a bug in this code or a buffer overflow
                // in the code using the memory.
                FAMOUSO_ASSERT(*prev_ai_na_p == size);

                return 0;
            }

        public:
            /// Constructor
            RawStorage() : first_allocated(size) {
            }

            /*!
             * \brief Allocate memory
             * \param bytes Number of bytes to allocate
             * \return Pointer to allocated memory on success, NULL otherwise.
             */
            uint8_t * alloc(SizeT bytes) {
                SizeT * next_allocated;
                SizeT to_allocate;
                uint8_t * mem;

                // Find first fitting empty block. Returns NULL pointer if there is none.
                next_allocated = find_first_fit(bytes, to_allocate);

                if (next_allocated) {
                    // Create memory management information for newly allocated block
                    AllocInfo * ai = get_ai(to_allocate);
                    ai->block_length = bytes;
                    ai->next_allocated = *next_allocated;

                    // Update memory management information (of previous block / global)
                    *next_allocated = to_allocate;

                    // Return pointer to allocated memory block
                    mem = reinterpret_cast<uint8_t *>(ai) + sizeof(AllocInfo);

#ifdef RAWSTORAGE_LOG_USAGE
                    ::logging::log::emit() << "RawStorage: alloc " << ::logging::log::dec << (int)(bytes)
                    << " bytes  ->  offset " << (int)to_allocate
                    << ::logging::log::hex << ", address "
                    << reinterpret_cast<unsigned long>(mem) << ::logging::log::endl;
#endif
                } else {
                    // Not enough free memory
                    mem = 0;
#ifdef RAWSTORAGE_LOG_USAGE
                    ::logging::log::emit() << "RawStorage: alloc " << ::logging::log::dec << (int)(bytes)
                    << " bytes failed" << ::logging::log::endl;

#endif
                }

#ifdef RAWSTORAGE_FULL_MEM_INFO
                print_mem_info();
#endif

                return mem;
            }

            /*!
             * \brief Free memory
             * \param p Pointer to memory returned by previous call to alloc()
             *
             * Invalid p catched by assert.
             */
            void free(uint8_t * p) {
                SizeT * next_allocated = find_allocated_block(p);

                // Check if we found a block (if pointer p is valid)
                FOR_FAMOUSO_ASSERT_ONLY(bool valid_pointer = next_allocated);
#ifdef RAWSTORAGE_LOG_USAGE
                if (!valid_pointer) {
                    ::logging::log::emit() << "RawStorage: free address "
                    << ::logging::log::hex << reinterpret_cast<unsigned long>(p)
                    << " failed. Not alloced or freed previously."
                    << ::logging::log::endl;
                }
#endif
                FAMOUSO_ASSERT(valid_pointer);

                // Alloc info to be freed... do not overwrite it
                AllocInfo * ai_to_free = get_ai(*next_allocated);

                // Update memory management information (of previous block / global)
                *next_allocated = ai_to_free->next_allocated;

#ifdef RAWSTORAGE_LOG_USAGE
                ::logging::log::emit() << "RawStorage: free address " << ::logging::log::hex << reinterpret_cast<unsigned long>(p)
                << ", offset " << ::logging::log::dec << (int)(p - data - sizeof(AllocInfo))
                << " (" << (int)ai_to_free->block_length << " bytes)" << ::logging::log::endl;
#endif
#ifdef RAWSTORAGE_FULL_MEM_INFO
                print_mem_info();
#endif
            }

            /*!
             * \brief Return length of allocated memory block
             * \param p Pointer to memory returned by previous call to alloc()
             *
             * Does not check if pointer p is valid!
             */
            SizeT get_length(uint8_t * p) {
                AllocInfo * ai = get_ai(p - data - sizeof(AllocInfo));
                return ai->block_length;
            }

#ifdef RAWSTORAGE_FULL_MEM_INFO
        private:

            /// Print info about empty block between offset first and last
            void print_empty(SizeT first, SizeT last) {
                ::logging::log::emit() << ::logging::log::tab << ::logging::log::dec << (int)first << "-"
                << (int)last << " Empty" << ::logging::log::endl;
            }

            /// Print info about allocated block at offset first
            void print_ai(SizeT first) {
                AllocInfo * ai = get_ai(first);
                SizeT last = first + sizeof(AllocInfo) + ai->block_length - 1;
                ::logging::log::emit() << ::logging::log::tab
                << ::logging::log::dec << (int)first << "-" << (int)last
                << ::logging::log::hex << " Allocated (address="
                << reinterpret_cast<unsigned long>(data + first + sizeof(AllocInfo))
                << ::logging::log::dec << ", length=" << (int)ai->block_length
                << ", next_allocated=" << (int)ai->next_allocated << ")" << ::logging::log::endl;

            }

        public:
            /// Print info about this RawStorage and list allocated and free blocks
            void print_mem_info() {
                ::logging::log::emit() << "RawStorage: size=" << size << ", sizeof(SizeT)=" << sizeof(SizeT)
                << ", first_allocated=" << (int)first_allocated << ::logging::log::endl;

                if (first_allocated != 0)
                    print_empty(0, first_allocated - 1);

                SizeT curr_ai = first_allocated;
                AllocInfo * ai = get_ai(curr_ai);
                while (curr_ai < size) {

                    print_ai(curr_ai);

                    SizeT behind = curr_ai + sizeof(AllocInfo) + ai->block_length;
                    if (behind != ai->next_allocated)
                        print_empty(behind, ai->next_allocated - 1);

                    curr_ai = ai->next_allocated;
                    ai = get_ai(curr_ai);
                }
                FAMOUSO_ASSERT(curr_ai == size);

                ::logging::log::emit() << ::logging::log::endl;
            }
#endif
    };




} // namespace object


#endif // __RAWSTORAGE_H_8AC69FC602CD2C__

