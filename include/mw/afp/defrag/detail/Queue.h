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


#ifndef __QUEUE_H_D5F01F9217D30D__
#define __QUEUE_H_D5F01F9217D30D__


#include "mw/afp/Config.h"
#include "object/RingBuffer.h"
#if !defined(__NO_STL__)
#include <list>
#else
#include "assert/staticerror.h"
#endif


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {
                namespace detail {

                    /*!
                     *  \brief  Queue of constant size
                     *  \tparam T   Item type
                     *  \tparam N   Max. item count
                     */
                    template < typename T, unsigned int N >
                    class Queue : public object::RingBuffer<T, N> {

                            typedef object::RingBuffer<T, N> Base;

                        public:

                            /// Push an item at the end of the queue
                            void push(const T & e) {
                                Base::push_back(e);
                            }

                            /// Pop first item
                            void pop() {
                                Base::pop_front();
                            }

                            /// Returns whether the queue is full
                            bool full() {
                                return Base::is_full();
                            }

                            /// Returns whether the queue is empty
                            bool empty() {
                                return Base::is_empty();
                            }
                    };

#if !defined(__NO_STL__)

                    /*!
                     *  \brief  Queue of dynamic size
                     *  \tparam T   Item type
                     */
                    template < typename T >
                    class Queue<T, dynamic> : public std::list<T> {

                            typedef std::list<T> Base;

                        public:

                            /// Push an item at the end of the queue
                            void push(const T & e) {
                                Base::push_back(e);
                            }

                            /// Pop first item
                            void pop() {
                                Base::pop_front();
                            }

                            /// Returns whether the queue is full
                            bool full() {
                                return false;
                            }
                    };

#else

                    template < typename T >
                    class Queue<T, dynamic> {
                        FAMOUSO_STATIC_ASSERT_ERROR(false, dynamic_Queue_not_supported_on_this_platform, ());
                    };

#endif

                } // namespace detail
            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __QUEUE_H_D5F01F9217D30D__

