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
 *
 * $Id$
 *
 ******************************************************************************/

inline Chain* Queue::ending() const {
    return tail;
}

inline void Queue::ending(const Chain& item) {
    tail = (Chain*) & item;
}

inline void Queue::adjust() {
    ending(*(Chain*)this);
}

inline Queue::Queue() : Stack() {
    adjust();
}

inline Queue::Queue(const Queue& copy) : Stack(copy) {
    tail = copy.tail;
}

inline Queue& Queue::operator = (const Queue & copy) {
    *(Stack*)this = (const Stack&)copy;
    tail = copy.tail;
    return *this;
}

inline void Queue::detach() {
    Stack::detach(); // remove head element
    if (!select()) { // queue is empty...
        adjust(); // ...adjust tail pointer
    }
}

inline void Queue::couple(Chain& item) {
    ending()->select(&item); // link tail element with item
    ending(item);   // let tail point to appended item
}

inline void Queue::append(Chain& item) {
    item.select(0);   // ensure terminated list
    couple(item);   // add item
}

inline Chain* Queue::remove() {
    register Chain* item = Stack::remove();
    adjust();   // adjust tail pointer
    return item;   // return old head pointer
}

inline int Queue::intact() const {
    return (ending() == (Chain*)this) ?
           select() != 0 : search(ending()) != 0;
}

inline Chain* Queue::unlink() {
    register Chain* item;
    if ((item = select())) {  // queue is non-empty...
        detach();  // ...remove head item
    }
    return item;
}

inline void Queue::remove(const Chain& item) {
    register Chain* wrap = search(&item);
    if (wrap) {   // present, container pointer is valid
        wrap->detach();
        if (!wrap->select()) { // tail element removed, adjust tail
            ending(*wrap);
        }
    }
}

inline void Queue::insert(Chain& item) {
    Stack::insert(item);  // insert item at head of queue
    if (ending() == (Chain*)this) { // 1st element queued...
        ending(item);  // ...adjust tail pointer
    }
}

inline void Queue::insert(const Queue& item) {
    if (select()) {
        if (item.select()) {
            item.ending()->select(select());
            select(item.select());
        }
    } else {
        if (item.select()) {
            ending(*item.ending());
            select(item.select());
        }
    }
}

inline void Queue::append(const Queue& item) {
    if (select()) {
        if (item.select()) {
            ending()->select(item.select());
            ending(*item.ending());
        }
    } else {
        if (item.select()) {
            select(item.select());
            ending(*item.ending());
        }
    }
}
