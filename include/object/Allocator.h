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
* $Id: $
*
******************************************************************************/

#ifndef __ALLOCATOR_H_811BED41A54BF9__
#define __ALLOCATOR_H_811BED41A54BF9__


#include <stdint.h>

#include "object/RawStorage.h"
#include "object/PlacementNew.h"

// Used for RawStorageAllocator
#include "logging/singleton.h"


/*!
 * \file
 * \brief Memory allocators supporting uninitialized allocation and
 *        %object construction.
 *
 * \par Introduction:
 *
 * This file defines two allocators:
 * - object::RawStorageAllocator which impelments allocation from a constant size
 *   memory pool. It's suitalble for the AVR platform, but may be used on other
 *   platforms as well.
 * - object::NewAllocator uses standard C++ new and delete operators for memory
 *   management. It's not available for AVR platform.
 *
 * This file also defines a default allocator: object::Allocator.
 *
 * \par Examples using default allocator:
 * - allocate unintialized raw buffer (100 bytes)
 *   \code
 *   uint8_t * p = object::Allocator::alloc(100);
 *   // ...
 *   object::Allocator::free(p);
 *   \endcode
 * - allocate unintialized \c int array with \c n elements
 *   \code
 *   uint8_t * p = object::Allocator::alloc_array<int>(n);
 *   // ...
 *   object::Allocator::free_array(p);
 *   \endcode
 * - create an %object in memory allocated by object::Allocator
 *   \code
 *   MyObject * p = new (object::Allocator()) MyObject(constructor_argument);
 *   // ...
 *   object::Allocator::destroy(p);
 *   \endcode
 */


namespace object {

    /*!
     * \brief Allocator allocating from RawStorage
     * \tparam tag Type not used for internal purposes but to allow multiple seperated
     *             RawStorage pools to be used for allocation. Allocators with the same
     *             tag share one RawStorage %object (implemented as singleton).
     * \tparam mem_size Size of RawStorage memory pool
     * \see object/Allocator.h
     */
    template <class tag, uint64_t mem_size>
    class RawStorageAllocator {

            typedef RawStorage<mem_size> ThisType;
        public:

            /// Type to use for sizes
            typedef typename ThisType::SizeT SizeT;

        protected:

            /// Singleton instance of RawStorage
            static ThisType & instance() {
                return logging::detail::singleton<ThisType>::instance();
            }

        public:

            /*!
             * \brief Allocate raw memory (no constructor call)
             * \param bytes Number of bytes to allocate
             * \return Pointer to uninitialized allocated memory or NULL on error.
             */
            static uint8_t * alloc(SizeT bytes) {
                return instance().alloc(bytes);
            }

            /*!
             * \brief Free memory previously allocated by alloc()
             * \param p Pointer to memory to free
             */
            static void free(uint8_t * p) {
                instance().free(p);
            }

            /*!
             * \brief Allocate array (no constructor call)
             * \tparam T Array element type
             * \param num Number of T items to allocate
             * \return Pointer to uninitialized allocated memory or NULL on error.
             */
            template <typename T>
            static T * alloc_array(SizeT num) {
                return reinterpret_cast<T *>(alloc(sizeof(T) * num));
            }

            /*!
             * \brief Free array previously allocated by alloc_array()
             * \param p Pointer to memory to free
	     * \note Does not call destuctors!
             */
            template <typename T>
            static void free_array(T * p) {
                return free(reinterpret_cast<uint8_t *>(p));
            }

            /*!
             * \brief Destruct and free %object previously allocated by allocator new
             * \param p Pointer to %object to free
             */
            template <typename T>
            static void destroy(T * p) {
                p->~T();
                free(reinterpret_cast<uint8_t *>(p));
            }
    };


#ifndef __AVR__
    /*!
     * \brief Allocator using standard C++ new and delete operators
     * \see object/Allocator.h
     */
    class NewAllocator {
        public:
            /// Type to use for sizes
            typedef unsigned int SizeT;

            /*!
             * \brief Allocate raw memory (no constructor call)
             * \param bytes Number of bytes to allocate
             * \return Pointer to uninitialized allocated memory or NULL on error.
             */
            static uint8_t * alloc(SizeT bytes) {
                return new(std::nothrow) uint8_t [bytes];
            }

            /*!
             * \brief Free memory previously allocated by alloc()
             * \param p Pointer to memory to free
             */
            static void free(uint8_t * p) {
                delete [] p;
            }

            /*!
             * \brief Allocate array (no constructor call)
             * \tparam T Array element type
             * \param num Number of T items to allocate
             * \return Pointer to uninitialized allocated memory or NULL on error.
             */
            template <typename T>
            static T * alloc_array(SizeT num) {
                return reinterpret_cast<T*>(alloc(sizeof(T) * num));
            }

            /*!
             * \brief Free array previously allocated by alloc_array()
             * \param p Pointer to memory to free
	     * \note Does not call destuctors!
             */
            template <typename T>
            static void free_array(T * p) {
                return free(reinterpret_cast<uint8_t *>(p));
            }

            /*!
             * \brief Destruct and free %object previously allocated by allocator new
             * \param p Pointer to %object to free
             */
            template <typename T>
            static void destroy(T * p) {
                p->~T();
                free(reinterpret_cast<uint8_t *>(p));
            }
    };
#endif


#ifndef DEFAULT_RAWSTORAGEALLOCATOR_MEM_SIZE

#ifdef __AVR__
#define DEFAULT_RAWSTORAGEALLOCATOR_MEM_SIZE 0xff
#else
    /*!
     * \brief Memory size for RawStorageAllocator used as default Allocator on AVR platform
     *
     * Define it before including Allocator.h to make another amout of memory available
     * for allocations via Allocator.
     *
     * Default value is 255 bytes on AVR platform, 65535 bytes otherwise.
     */
#define DEFAULT_RAWSTORAGEALLOCATOR_MEM_SIZE 0xffff
#endif

#endif

    /*!
     * \brief Class used as tag for RawStorageAllocator used as default Allocator on AVR platform
     */
    class DefaultRawStorageAllocatorTag;

#ifdef __AVR__
    typedef RawStorageAllocator<DefaultRawStorageAllocatorTag, DEFAULT_RAWSTORAGEALLOCATOR_MEM_SIZE> Allocator;
#else

    /*!
     * \brief Default allocator
     *
     * This allocator is used by several classes (e.g. in AFP implementation) if you do
     * not explicitly configure them to use another one.
     *
     * On AVR platform it is a RawStorageAllocator with DEFAULT_RAWSTORAGEALLOCATOR_MEM_SIZE bytes of
     * memory. For other platforms it is the NewAllocator.
     */
    typedef NewAllocator Allocator;
#endif

} // namespace object


/*!
 * \brief Allocate a new %object using RawStorageAllocator and call constructor
 * \return Pointer to constructed %object. NULL if out of memory.
 *
 * \par Examples:
 *
 * - using default allocator (object::Allocator), one constructor parameter
 *   \code
 *   MyObject * p = new (object::Allocator()) MyObject(constructor_argument);
 *   // ...
 *   object::Allocator::destroy(p);
 *   \endcode
 * - using own allocator, no constructor parameters
 *   \code
 *   MyObject2 * p = new (MyAllocator()) MyObject2;
 *   // ...
 *   MyAllocator::destroy(p);
 *   \endcode
 */
template <typename tag, uint64_t mem_size>
void * operator new(std::size_t size, const object::RawStorageAllocator<tag, mem_size> & allocator) throw() {
    return allocator.alloc(size);
}


#ifndef __AVR__
/*!
 * \brief Allocate a new %object using NewAllocator and call constructor
 * \return Pointer to constructed %object. NULL if out of memory.
 *
 * \par Examples:
 *
 * - using default allocator (object::Allocator), one constructor parameter
 *   \code
 *   MyObject * p = new (object::Allocator()) MyObject(constructor_argument);
 *   // ...
 *   object::Allocator::destroy(p);
 *   \endcode
 * - using own allocator, no constructor parameters
 *   \code
 *   MyObject2 * p = new (MyAllocator()) MyObject2;
 *   // ...
 *   MyAllocator::destroy(p);
 *   \endcode
 */
void * operator new(std::size_t size, const object::NewAllocator & allocator) throw() {
    return allocator.alloc(size);
}
#endif


#endif // __ALLOCATOR_H_811BED41A54BF9__

