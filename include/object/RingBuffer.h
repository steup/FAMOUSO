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

#ifndef __RINGBUFFER_H_E7CCEE63FC08A7__
#define __RINGBUFFER_H_E7CCEE63FC08A7__


#include "debug.h"
#include "config/type_traits/SmallestUnsignedTypeSelector.h"
#include "config/type_traits/ExpandedRangeTypeSelector.h"


namespace object {

    /*!
     *  \brief  Ring buffer (constant size)
     *  \tparam T   Type of elements
     *  \tparam N   Maximum count of elements
     *  \author Philipp Werner
     */
    template <class T, unsigned int N>
    class RingBuffer {
        public:

            /// Type used for internal indexing
            typedef typename SmallestUnsignedTypeSelector<N>::type SizeT;

        protected:

            /// Type with larger range than SizeT to handle overflows
            typedef typename ExpandedRangeTypeSelector<SizeT>::type DoubleSizeT;

            /// %Array containing elements
            T buffer[N];

            /// %Array index of first element (always < N)
            SizeT first;

            /// Number of elements currently used (always <= N)
            SizeT count;


            /*!
             *  \brief  Get index of element
             *  \param  num Number of element behind first (first = 0, last = count-1)
             *              Must be smaller than \c N.
             *  \return Index of element in \c buffer
             */
            SizeT get_idx(SizeT num) const {
                FAMOUSO_ASSERT(num < N);
                // Avoid overflow of SizeT
                DoubleSizeT idx = (DoubleSizeT)(first) + (DoubleSizeT)(num);
                // Handle overflow over element count (N)
                // Avoid modulo because it is expensive on AVR
                if (idx < N)
                    return idx;
                else
                    return idx - N;
            }

            /// Increments \c first
            void increment_first() {
                // Avoid modulo because it is expensive on AVR
                if (first == N-1)
                    first = 0;
                else
                    first++;
            }

            /// Decrements \c first
            void decrement_first() {
                // Avoid modulo because it is expensive on AVR
                if (first == 0)
                    first = N-1;
                else
                    first--;
            }

            /// Return index of last element
            SizeT get_last() const {
                FAMOUSO_ASSERT(!is_empty());
                return get_idx(count - 1);
            }

            /// Return index of element behind last element
            SizeT get_behind_last() const {
                return get_idx(count);
            }

            /// Return index of element in front of first element
            SizeT get_infront_first() const {
                return get_idx(N-1);
            }

        public:
            /// Constructor
            RingBuffer() :
                    first(0), count(0) {
            }

            /// Returns current count of elements
            SizeT get_count() const {
                return count;
            }

            /// Returns true if there are no elements inside
            bool is_empty() const {
                return count == 0;
            }

            /// Returns true if no more elements can be added
            bool is_full() const {
                return count == N;
            }

            /*!
             *  \brief  Returns reference to the first element
             *  \pre    is_empty() returns false
             */
            T & front() {
                FAMOUSO_ASSERT(!is_empty());
                return buffer[first];
            }

            /*!
             *  \brief  Returns reference to the first element
             *  \pre    is_empty() returns false
             */
            const T & front() const {
                FAMOUSO_ASSERT(!is_empty());
                return buffer[first];
            }

            /*!
             *  \brief  Returns reference to the last element
             *  \pre    is_empty() returns false
             */
            T & back() {
                return buffer[get_last()];
            }

            /*!
             *  \brief  Returns reference to the last element
             *  \pre    is_empty() returns false
             */
            const T & back() const {
                return buffer[get_last()];
            }

            /*!
             *  \brief  Returns reference to element \p i
             *  \param  i   Index of element. Zero for the first element,
             *              \c count-1 for the last.
             */
            T & operator[] (SizeT i) {
                return buffer[get_idx(i)];
            }

            /*!
             *  \brief  Returns reference to element \p i
             *  \param  i   Index of element. Zero for the first element,
             *              \c count-1 for the last.
             */
            const T & operator[] (SizeT i) const {
                return buffer[get_idx(i)];
            }

            /*!
             *  \brief  Adds an element in front of the first one
             *  \param  obj Object to insert
             *  \pre    is_full() returns false
             */
            void push_front(const T & obj) {
                FAMOUSO_ASSERT(!is_full());
                buffer[get_infront_first()] = obj;
                decrement_first();
                count++;
            }

            /*!
             *  \brief  Adds an element behind the last one
             *  \param  obj Object to insert
             *  \pre    is_full() returns false
             */
            void push_back(const T & obj) {
                FAMOUSO_ASSERT(!is_full());
                buffer[get_behind_last()] = obj;
                count++;
            }

            /*!
             *  \brief  Removes the first element
             *  \pre    is_empty() returns false
             */
            void pop_front() {
                FAMOUSO_ASSERT(!is_empty());
                increment_first();
                count--;
            }

            /*!
             *  \brief  Removes the last element
             *  \pre    is_empty() returns false
             */
            void pop_back() {
                FAMOUSO_ASSERT(!is_empty());
                count--;
            }
    };

} // namespace object


#endif // __RINGBUFFER_H_E7CCEE63FC08A7__

