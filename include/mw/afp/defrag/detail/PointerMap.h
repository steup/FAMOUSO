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

#ifndef __POINTERMAP_H_8DCAB52FEC8A52__
#define __POINTERMAP_H_8DCAB52FEC8A52__

#include "mw/afp/Config.h"
#include <string.h>
#if !defined(__NO_STL__)
#include <map>
#else
#include "assert/staticerror.h"
#endif


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {
                namespace detail {

                    /*!
                     *  \brief  Pointer container of constant size
                     *  \tparam KeyT    Key data type
                     *  \tparam ItemT   Dereferenced item data type (the type pointed to).
                     *                  Must have a member function get_key() which returns
                     *                  KeyT.
                     *  \tparam N       Max. count of items
                     *
                     *  This container is not implemented as a map and does not check
                     *  uniqueness of keys.
                     */
                    template <class KeyT, class ItemT, unsigned int N>
                    class PointerMap {
                            /// Item pointer array
                            ItemT * array[N];

                            /// Get first non-empty item starting from i
                            ItemT ** get_next(ItemT ** i) {
                                while (i != &array[N] && *i == 0) {
                                    i++;
                                }
                                return i;
                            }

                        public:

                            /// Type definition of an iterator
                            class iterator {
                                    PointerMap & pm;
                                    ItemT ** item;
                                public:
                                    iterator(PointerMap & pm, ItemT ** item) :
                                            pm(pm), item(item) {
                                    }

                                    iterator operator = (const iterator & i) {
                                        pm = i.pm;
                                        item = i.item;
                                        return iterator(pm, item);
                                    }

                                    /// Prefix increment iterator
                                    iterator & operator ++ () {
                                        item = pm.get_next(item + 1);
                                        return *this;
                                    }

                                    /// Postfix increment iterator
                                    iterator operator ++ (int) {
                                        ItemT ** old = item;
                                        item = pm.get_next(item + 1);
                                        return iterator(pm, old);
                                    }

                                    /// Check for equality
                                    bool operator == (const iterator& i) const {
                                        return item == i.item;
                                    }

                                    /// Check for inequality
                                    bool operator != (const iterator& i) const {
                                        return item != i.item;
                                    }

                                    /// Derefernece operator
                                    ItemT *& operator * () {
                                        return *item;
                                    }
                            };

                            /// Constructor
                            PointerMap() {
                                memset(array, 0, sizeof(array));
                            }

                            /*!
                             *  \brief  Insert an item
                             *  \param  item    Pointer to insert
                             *  \return Returns whether item was inserted successfully
                             */
                            bool insert(ItemT * item) {
                                for (ItemT ** i = array; i != array + N; i++) {
                                    if (*i == 0) {
                                        *i = item;
                                        return true;
                                    }
                                }
                                return false;
                            }

                            /*!
                             *  \brief  Erase an item
                             *  \param  it  Iterator pointing to the item to remove
                             */
                            void erase(iterator it) {
                                *it = 0;
                            }

                            /*!
                             *  \brief  Erase first item matching a key
                             *  \param  key Key identifying the item to remove
                             */
                            void erase(const KeyT & key) {
                                ItemT ** end = array + N;
                                for (ItemT ** i = array; i != end; i++) {
                                    if (*i != 0 && key == (*i)->get_key()) {
                                        *i = 0;
                                        return;
                                    }
                                }
                            }

                            /*!
                             *  \brief  Find first item matching a key
                             *  \param  key Key of the item to find
                             *  \return Iterator pointing to the item. If there is no
                             *          item with the given key, end() is returned.
                             */
                            iterator find(const KeyT & key) {
                                ItemT ** end = array + N;
                                for (ItemT ** i = array; i != end; i++) {
                                    if (*i != 0 && key == (*i)->get_key()) {
                                        return iterator(*this, i);
                                    }
                                }
                                return this->end();
                            }

                            /// Returns iterator pointing to the first item
                            iterator begin() {
                                return iterator(*this, get_next(&array[0]));
                            }

                            /// Returns iterator pointing behind the last item
                            iterator end() {
                                return iterator(*this, &array[N]);
                            }
                    };



#if !defined(__NO_STL__)

                    /*!
                     *  \brief  Pointer container of dynamic size
                     *  \tparam KeyT    Key data type
                     *  \tparam ItemT   Dereferenced item data type (the type pointed to).
                     *                  Must have a member function get_key() which returns
                     *                  KeyT.
                     *
                     *  This container is implemented using std::map.
                     */
                    template <class KeyT, class ItemT>
                    class PointerMap<KeyT, ItemT, dynamic> : private std::map<KeyT, ItemT *> {

                            typedef std::map<KeyT, ItemT *> Base;

                        public:

                            /// Iterator type
                            class iterator {
                                public:
                                    typename Base::iterator it;

                                    iterator(const typename Base::iterator & it) :
                                            it(it) {
                                    }

                                    iterator operator = (const iterator & i) {
                                        it = i.it;
                                        return iterator(it);
                                    }

                                    /// Prefix increment iterator
                                    iterator & operator ++ () {
                                        ++it;
                                        return *this;
                                    }

                                    /// Postfix increment iterator
                                    iterator operator ++ (int) {
                                        typename Base::iterator old = it;
                                        ++it;
                                        return iterator(old);
                                    }

                                    /// Check for equality
                                    bool operator == (const iterator& i) const {
                                        return it == i.it;
                                    }

                                    /// Check for inequality
                                    bool operator != (const iterator& i) const {
                                        return it != i.it;
                                    }

                                    /// Derefernece operator
                                    ItemT *& operator * () {
                                        return it->second;
                                    }
                            };

                            /*!
                             *  \brief  Insert an item
                             *  \param  item    Pointer to insert
                             *  \return Returns whether item was inserted successfully
                             */
                            bool insert(ItemT * item) {
                                std::pair<iterator, bool> res = Base::insert(typename Base::value_type(item->get_key(), item));
                                return res.second;
                            }

                            /*!
                             *  \brief  Erase an item
                             *  \param  it  Iterator pointing to the item to remove
                             */
                            void erase(iterator it) {
                                Base::erase(it.it);
                            }

                            /*!
                             *  \brief  Erase item matching a key
                             *  \param  key Key identifying the item to remove
                             */
                            void erase(const KeyT & key) {
                                Base::erase(key);
                            }

                            /*!
                             *  \brief  Find item matching a key
                             *  \param  key Key of the item to find
                             *  \return Iterator pointing to the item. If there is no
                             *          item with the given key, end() is returned.
                             */
                            iterator find(const KeyT & key) {
                                return Base::find(key);
                            }

                            /// Returns iterator pointing to the first item
                            iterator begin() {
                                return iterator(Base::begin());
                            }

                            /// Returns iterator pointing behind the last item
                            iterator end() {
                                return iterator(Base::end());
                            }
                    };

#else

                    template <class KeyT, class ItemT>
                    class PointerMap<KeyT, ItemT, dynamic> {
                        FAMOUSO_STATIC_ASSERT_ERROR(false, dynamic_PointerMap_not_supported_on_this_platform, ());
                    };

#endif

                } // namespace detail
            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __POINTERMAP_H_8DCAB52FEC8A52__

