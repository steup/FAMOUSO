/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
 *    $Id$
 *
 ******************************************************************************/
#ifndef __ConstantSizeList_h__
#define __ConstantSizeList_h__

#include <stdint.h>
#include "object/Array.h"
#include "object/Uninitialized.h"

namespace object {

    /*! \brief ConstantSizeList is a simple abstraction of a list with a maximum
     *         length of N elements, supporting an iterator interface.
     *
     *         The N elements are allocated withing the list itself, thus none
     *         additional memory allocator facility is needed. Furthermore, it
     *         provides methodes for getting new elements and for the deletion
     *         of such one.
     *
     * \tparam T is the element type that is stored
     * \tparam N is the count of elements that is stored
     * \tparam callDestructor enables switiching between the destruction of all
     *         objects residing in the underlying container or doing nothing, in
     *         order to safe program space. In embedded environments it is
     *         sometimes possible to forbear destruction.
     *
     *  \todo ConstantSizeList supports up to 255 elements, at the moment. To
     *        avoid false usage, the parameter N should be checked against this
     *        boundary, and if N is defined to high, a compile time error has
     *        to arise. It is also possible to invent an implementation, that
     *        configures automatically the correct types to circumvent the
     *        shortcoming.
     */
    template < typename T,
               unsigned int N,
               bool callDestructor = true >
    class ConstantSizeList {

            //! Storage for the contained elements
            Array< Uninitialized<T>, N > content;

            //! how much additional space is needed for the management information
            enum {
                used_space = (N / 8) + ((N % 8) ? 1 : 0)
            };

            /*! \brief management information describing which slot of the
             *         Array content is used.
             */
            uint8_t used[used_space];

        public:
            //! Type definition of instatiated type
            typedef T value_type;

            //! Type definition of pointer to intantiated type
            typedef T* pointer;

            //! Type definition of a reference to intantiated type
            typedef T& reference;

            //! Type definition of an iterator interface
            struct ConstantSizeListIterator;
            typedef ConstantSizeListIterator iterator;

            ConstantSizeList () {
                for (uint8_t i = 0;i < used_space;++i) {
                    used[i] = 0;
                }
            }

            ~ConstantSizeList () {
                if (callDestructor) {
                    iterator i = begin();
                    while (i != end()) {
                        delElement (&*i);
                        ++i;
                    }
                }
            }

        public:
            //! Get an iterator which references this chain element
            iterator begin () {
                return iterator(*this, getFirstElement());
            }

            //! Get an iterator which marks the end of the \ref Chain
            iterator end () {
                return iterator(*this, N);
            }

            //! defines a usual iterator
            class ConstantSizeListIterator {
                    //! Reference to the related storage
                    ConstantSizeList&  _storage;
                    //! Pointer to the current position of the iterator
                    uint8_t _currentPos;

                public:
                    //! Type definition of instatiated type
                    typedef typename ConstantSizeList::value_type value_type;

                    //! Type definition of pointer to intantiated type
                    typedef typename ConstantSizeList::pointer pointer;

                    //! Type definition of a reference to intantiated type
                    typedef typename ConstantSizeList::reference reference;

                    ConstantSizeListIterator (ConstantSizeList& s, const uint8_t pos) :
                            _storage(s), _currentPos(pos) {
                    }

                    ConstantSizeListIterator operator = (const ConstantSizeListIterator& i) {
                        _storage = i._storage;
                        _currentPos = i._currentPos;
                        return ConstantSizeListIterator(_storage, _currentPos);
                    }

                    /*! Increment iterator
                     *
                     * \return an iterator
                     */
                    ConstantSizeListIterator operator ++ () {
                        _currentPos = _storage.getNextElement(_currentPos + 1);
                        return ConstantSizeListIterator(_storage, _currentPos);
                    }

                    /*! Compare two iterators
                     *
                     * \return \a true if iterators are equal
                     */
                    bool operator == (const ConstantSizeListIterator& i) const {
                        return _currentPos == i._currentPos;
                    }

                    /*! Compare two iterators
                     *
                     * \return \a true if iterators are not equal
                     */
                    bool operator != (const ConstantSizeListIterator& i) const {
                        return _currentPos != i._currentPos;
                    }

                    /*! Derefernece operator
                     *
                     * \return the content of the position  the
                     *  iterator is pointing at
                     */
                    value_type& operator * () const {
                        return reinterpret_cast<value_type&>(_storage.content[_currentPos]);
                    }

            };

            //! Removes the specified element from the storage.
            void delElement (const T* value) {
                for (uint8_t i = 0; i < N;++i)
                    if ( value == reinterpret_cast<T*>(&content[i]) ) {
                        reinterpret_cast<T*>(&content[i])->~T();
                        used[i/8] = used[i/8] & (~(0x80 >> (i % 8)));
                        return;
                    }
            }

            /*! \brief Delivers a new Element
             *
             * \return a pointer to an element or Null pointer if there are no
             *         free elements
             */
            T* newElement () {
                for (uint8_t i = 0;i < used_space;++i)
                    if ( used[i] != 0xff ) {
                        uint8_t mask = 0x80;
                        for (uint8_t j = 0;j < 8 ; ++j)
                            if (!(used[i] & mask)) {
                                used[i] |= mask;
                                return new(&content[(i*8+j)]) T();
                            } else
                                mask >>= 1;
                    }
                return 0;
            }

        private:

            /*! \brief searches the first used element
             *
             *         provides helper functionality for the iterator interface
             */
            uint8_t getFirstElement () {
                return getNextElement(0);
            }

            /*! \brief searches the next used element
             *
             *         provides helper functionality for the iterator interface
             *
             *  \param t at that position the search after the next used element
             *         is started.
             */
            uint8_t getNextElement (uint8_t t) {
                uint8_t start = t % 8;
                for (uint8_t i = t / 8;i < used_space;++i) {
                    if ( used[i] ) {
                        uint8_t mask = 0x80 >> start;
                        for (uint8_t j = start;j < 8 ; ++j) {
                            if ((used[i] & mask)) {
                                return (i*8 + j);
                            } else {
                                mask >>= 1;
                            }
                        }
                        start = 0;
                    }
                }
                return N;
            }
    };

} /* namespace object */

#endif

