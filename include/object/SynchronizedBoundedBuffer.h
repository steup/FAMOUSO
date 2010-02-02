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
 *
 * $Id$
 *
 ******************************************************************************/

#ifndef __SynchronizedBoundedBuffer_h__
#define __SynchronizedBoundedBuffer_h__

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/circular_buffer.hpp>
#include <deque>
#include "debug.h"

namespace object {

    template <class T>
    class SynchronizedBoundedBuffer {
        public:

            typedef std::deque<T> container_type;
            typedef typename container_type::size_type size_type;
            typedef typename container_type::value_type value_type;

            explicit SynchronizedBoundedBuffer(size_type capacity) : m_capacity(capacity) {}

            void push_front(const value_type& item) {
                boost::mutex::scoped_lock lock(m_mutex);
                m_not_full.wait(lock, boost::bind(&SynchronizedBoundedBuffer<value_type>::is_not_full, this));
                m_container.push_front(item);
                lock.unlock();
                m_not_empty.notify_one();
            }

            void pop_back(value_type* pItem) {
                boost::mutex::scoped_lock lock(m_mutex);
                m_not_empty.wait(lock, boost::bind(&SynchronizedBoundedBuffer<value_type>::is_not_empty, this));
                *pItem = m_container.back();
                m_container.pop_back();
                lock.unlock();
                m_not_full.notify_one();
            }

            bool is_not_empty() const {
                return m_container.size() > 0;
            }
            bool is_not_full() const {
                return m_container.size() < m_capacity;
            }

        private:

            SynchronizedBoundedBuffer(const SynchronizedBoundedBuffer&);              // Disabled copy constructor
            SynchronizedBoundedBuffer& operator = (const SynchronizedBoundedBuffer&); // Disabled assign operator

            const size_type m_capacity;
            container_type m_container;
            boost::mutex m_mutex;
            boost::condition m_not_empty;
            boost::condition m_not_full;
    };

} /* namespace object */

#endif

