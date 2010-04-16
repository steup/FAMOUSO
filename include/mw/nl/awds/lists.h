/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *               2009-2010 Michael Kriese <kriese@cs.uni-magdeburg.de>
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
#ifndef __lists_hpp__
#define __lists_hpp__

#include <list>
#include "boost/shared_ptr.hpp"
#include "boost/noncopyable.hpp"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {

                /*! \brief a generic iterable container secured by a shared pointer.
                 *
                 *  This container can only be used within a shared pointer.
                 *
                 *  \tparam Type The type which the container should take.
                 *  \tparam ContainerType The type of the real container holding the elements.
                 */
                template< class Type, typename ContainerType=std::list<Type> >
                class Container: boost::noncopyable {

                    public:
                        /*! \copydoc Container
                         */
                        typedef boost::shared_ptr<Container<Type> > type;

                        /** \brief An iterator to loop over all elements contained. */
                        typedef typename ContainerType::iterator iterator;

                        /** \brief The type for the size of the container. */
                        typedef typename ContainerType::size_type size_type;

                        /** \brief Create a new Container secured by a shared pointer.
                         *
                         * \return A new instance to a container packed in a shared pointer.
                         */
                        static type Create() {
                            type res = type(new Container<Type> ());
                            return res;
                        }

                        /** \brief Returns an iterator which points to the first element of the container.
                         *
                         *  \return An iterator to the first element.
                         */
                        iterator begin() {
                            return _list.begin();
                        }

                        /** \brief Returns an iterator which points behind the last element of the container.
                         *
                         *  \return An iterator behind the last element.
                         */
                        iterator end() {
                            return _list.end();
                        }

                        /** \brief Returns the number of elements contained.
                         *
                         *  \return The number of elements contained.
                         */
                        size_type size() const {
                            return _list.size();
                        }

                        /** \brief Add an element to the container.
                         *
                         * \param c The element to add.
                         */
                        void add(Type c) {
                            _list.push_back(c);
                        }

                        /** \brief Remove an element from the container.
                         *
                         *  \param c The element to remove.
                         */
                        void remove(Type c) {
                            _list.remove(c);
                        }

                        /** \brief Remove the element from the container to which the iterator points.
                         *
                         * \param it An iterator which has to point to an element of the container.
                         * \return The iterator pointing to the next element after the removed element, or to the end
                         */
                        iterator erase(iterator it) {
                            return _list.erase(it);
                        }

                        /** \brief Removes all elements from the container.
                         */
                        void clear() {
                            _list.clear();
                        }

                    private:
                        ContainerType _list; /**< The real container holding the elements */
                };

            } /* awds */
        } /* nl */
    } /* mw */
} /* famouso */
#endif /* __lists_hpp__ */
