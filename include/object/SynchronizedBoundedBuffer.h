#ifndef __SynchronizedBoundedBuffer_h__
#define __SynchronizedBoundedBuffer_h__

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/circular_buffer.hpp>
#include <deque>
#include <iostream>

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

