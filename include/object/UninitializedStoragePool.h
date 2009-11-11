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
#ifndef __UninitializedStoragePool_h__
#define __UninitializedStoragePool_h__

#include "object/Uninitialized.h"

namespace object {

    /*! \brief UninitializedStoragePool is an abstraction of an arbitrary
     *         storage structure (supporting a certain interface) with
     *         uninitialized data, providing an iterator interface.
     *
     *         It enables using uninitialized storage for objects of the
     *         certain type T to allow deferred calls of constructors.
     *         Further, it provides a  possibility to manage objects without
     *         default or assignment  constructors, which is impossible with
     *         standard container.
     *
     * \tparam T is the element type that is stored
     * \tparam C is the underlying storage structure
     * \tparam N is the count of elements that is stored and the default is eight
     * \tparam callDestructor enables switiching between the destruction of all
     *         objects residing in the underlying container or doing nothing, in
     *         order to safe program space. In embedded environments it is
     *         sometimes possible to forbear destruction.
     */
    template < typename T,
               template<typename, unsigned int, bool> class C,
               unsigned int N=8,
               bool callDestructor = true >
    class UninitializedStoragePool : public C< Uninitialized<T>, N, callDestructor > {

            //! Type definition of the underlying storage structure
            typedef C<Uninitialized<T> , N, callDestructor >   Storage;
        public:
            //! Type definition of instatiated type
            typedef T   value_type;

            //! Type definition of pointer to intantiated type
            typedef T*  pointer;

            //! Type definition of a reference to intantiated type
            typedef T&  reference;

            //! Type definition of an iterator
            class Iterator : public Storage::iterator {
                public:
                    /*! Derefernece operator
                     *
                     * \return the content of the position the iterator is pointing at
                     */
                    value_type& operator* () const {
                        return reinterpret_cast<value_type&>(Storage::iterator::operator*());
                    }

                private:
                    Iterator ();
            };
            typedef Iterator    iterator;

            ~UninitializedStoragePool () {
                if (callDestructor)
                    for ( iterator i = begin(); i != end(); i = begin() )
                        delElement (&*i);
            }

            //! Get an iterator which references the first element
            iterator begin () __attribute__((always_inline)) {
                typename Storage::iterator i = Storage::begin();
                return *static_cast<iterator*>(&i);
            }

            //! Get an iterator which marks the end of the Array
            iterator end () __attribute__((always_inline)) {
                typename Storage::iterator i = Storage::end();
                return *static_cast<iterator*>(&i);
            }

            //! Removes the specified element from the storage.
            void delElement (const pointer v) __attribute__((always_inline)) {
                Storage::delElement(reinterpret_cast<Uninitialized<value_type>*>(v));
                v->~value_type();
            }

            /*! \brief Delivers a new Element
             *
             * \return a pointer to an element or Null pointer if there are no
             *         free elements
             */
            value_type* newElement () __attribute__((always_inline)) {
                return reinterpret_cast<value_type*>(Storage::newElement());
            }
    };

} /* namespace object */

#endif

