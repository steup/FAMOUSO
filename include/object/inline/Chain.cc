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

#include "assert.h"

inline Chain::Chain() {
    link = 0;
}

inline Chain::Chain(Chain* item) {
    link = item;
}

//
// Return next chain item.
//
inline Chain* Chain::select() const {
    return link;
}

//
// Define next item according to actual parameter.
//
inline void Chain::select(Chain* item) {
    link = item;
}

//
// Detach next item from this chain instance.
//
inline void Chain::detach() {
    assert(select());
    select(select()->select());
}

//
// Append specified item at this chain instance.
//
inline void Chain::append(Chain& item) {
    item.select(select());  // append item at this item
    select(&item);   // make item new successor
}

//
// Unlink next item from chain. Update next pointer.
//
inline Chain* Chain::unlink() {
    register Chain* item;
    if ((item = select()) != 0) { // there is a successor, remove...
        detach();
    }
    return item;
}

//
// Search container instance of specified chain item. Once found,
// pointer to the container instance is returned. Otherwise, null
// will be delivered.
//
inline Chain* Chain::search(const Chain* item) const {
    register Chain* wrap = (Chain*)this;

    while (wrap && (wrap->select() != item)) {
        wrap = wrap->select();
    }

    return wrap;
}

//
// Remove entire chain and deliver head/next pointer.
//
inline Chain* Chain::remove() {
    register Chain* item = select();
    select(0);
    return item;
}

//
// Remove specified item from the chain.
//
inline void Chain::remove(const Chain& seek) {
    register Chain* wrap = search(&seek);
    if (wrap) {   // present, unlink from chain
        wrap->detach();
    }
}

//
// Deliver ending of this list of chain items.
//
inline Chain* Chain::ending() const {
    register Chain* item = (Chain*)this;
    while (item->select()) {
        item = item->select();
    }
    return item;
}




