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


#include <string.h>

#if !defined(__AVR__)
#include <map>
#endif


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {
                namespace detail {

                    // Stellt nicht die eindeutigkeit sicher
                    template <class KeyT, class ItemT, unsigned int N>
                    class PointerMap {

                            /// Item pointer array
                            ItemT * array[N];

                            /// Get first non-empty item starting from i
                            ItemT ** get_next(ItemT ** i) {
                                while (i != &array[N]) {
                                    if (*i != 0)
                                        break;
                                    else
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
                                    iterator operator ++ () {
                                        item = pm.get_next(item + 1);
                                        return iterator(pm, item);
                                    }

                                    /// Postfix increment iterator
                                    iterator operator ++ (int) {
                                        item = pm.get_next(item + 1);
                                        return iterator(pm, item);
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
                                    ItemT *& operator * () const {
                                        return *item;
                                    }
                            };

                            PointerMap() {
                                memset(array, 0, sizeof(array));
                            }

                            bool insert(ItemT * item) {
                                for (ItemT ** i = array; i != array + N; i++) {
                                    if (*i == 0) {
                                        *i = item;
                                        return true;
                                    }
                                }
                                return false;
                            }

                            void erase(iterator it) {
                                *it = 0;
                            }

                            void erase(const KeyT & key) {
                                ItemT ** end = array + N;
                                for (ItemT ** i = array; i != end; i++) {
                                    if (*i != 0 && key == (*i)->get_key()) {
                                        *i = 0;
                                        return;
                                    }
                                }
                            }

                            iterator find(const KeyT & key) {
                                ItemT ** end = array + N;
                                for (ItemT ** i = array; i != end; i++) {
                                    if (*i != 0 && key == (*i)->get_key()) {
                                        return iterator(*this, i);
                                    }
                                }
                                return this->end();
                            }

                            iterator begin() {
                                return iterator(*this, get_next(&array[0]));
                            }

                            iterator end() {
                                return iterator(*this, &array[N]);
                            }
                    };



#if !defined(__AVR__)

                    template <class KeyT, class ItemT>
                    class VarSizePointerMap : private std::map<KeyT, ItemT *> {

                            typedef std::map<KeyT, ItemT *> Base;

                        public:

                            typedef typename Base::iterator iterator;

                            bool insert(const ItemT * item) {
                                std::pair<iterator, bool> res = Base::insert(typename Base::value_type(item.get_key(), item));
                                return res->second;
                            }

                            void erase(const KeyT & key) {
                                Base::erase(key);
                            }

                            void erase(iterator it) {
                                Base::erase(it);
                            }

                            iterator find(const KeyT & key) {
                                return Base::find(key);
                            }

                            iterator begin() {
                                return Base::begin();
                            }

                            iterator end() {
                                return Base::end();
                            }
                    };

#else

                    template <class KeyT, class ItemT>
                    class VarSizePointerMap {
                        BOOST_MPL_ASSERT_MSG(false, VariableSizePointerMap_not_supported_on_this_platform, ());
                    };

#endif

                } // namespace detail
            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __POINTERMAP_H_8DCAB52FEC8A52__

