/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
#ifndef __DynamicList_h__
#define __DynamicList_h__

#include <list>
#include <algorithm>

namespace object {

    /*! \brief DynamicList is a simple abstraction of a list with a dynamic
     *         length, supporting an iterator interface.
     *
     *         It is derived from the std::List and supports the same interface
     *         as the ConstantSizeList to allow unification (e.g. same number of
     *         template parameters). Furthermore, it provides methodes for
     *         getting new elements and for the deletion of such one.
     *
     * \tparam T element type to be stored
     * \tparam N count element, but here it is ignored and only
     *         part due to providing the same interface
     * \tparam callDestructor enables switiching between the destruction of all
     *         objects residing in the underlying container or doing nothing,
     *         in order to safe program space. In embedded environments it is
     *         sometimes possible to forbear destruction. The parameter is
     *         ignored and only part due to providing the same interface
     */
    template < typename T,
               unsigned int N = 0,
               bool callDestructor = true >
    class DynamicList : public std::list<T> {
        public:

            /*! \brief Removes the specified element from the storage.
            */
            void delElement (const T* v) {
                erase(find(this->begin(), this->end(), *v));
            }

            /*! \brief Delivers a new Element
             *
             * \return a pointer to an element or zero if no element
             *         is created
             */
            T* newElement () {
                this->push_back(T());
                return &(this->back());
            }

    };

} /* namespace object */

#endif

