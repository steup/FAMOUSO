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

#include "famouso_bindings.h"

//  Events are stored in a simple cyclic buffer
//  if the buffer is full, events get lost

// SubscriberEC code
SubscriberEC::SubscriberEC(const subject_t &sub, uint32_t count) : first(0), last(0), max(count) {
    sec = new famouso::config::SEC(famouso::mw::Subject(sub));
    events = new Storage[count];
}
SubscriberEC::~SubscriberEC() {
    delete sec;
    delete [] events;
}

void SubscriberEC::notify() {
}

void SubscriberEC::cb(famouso::mw::api::SECCallBackData &cb) {
    if (!events[first].used) {
        events[first].used = 1;
        events[first].event.subject = cb.subject.value();
        events[first].event.len = cb.length;
        memcpy(events[first].event.data, cb.data, cb.length);
        first = (first + 1) % max;
        notify();
    }
}

int SubscriberEC::subscribe() {
    sec->subscribe();
    sec->callback.bind<SubscriberEC, &SubscriberEC::cb>(this);
    return 1;
}

int SubscriberEC::unsubscribe() {
    return 1;
}

// simple automaton for polling the correct event to
// the application
int SubscriberEC::poll(event_t &e) {
    if (events[last].used) {
        if (events[last].used == 2) {
            events[last].used = 0;
            last = (last + 1) % max;
        }
        if (events[last].used == 1) {
            events[last].used++;
            e = events[last].event;
            return 1;
        }
    }
    return 0;
}

// PublisherEC code
PublisherEC::PublisherEC(const subject_t &sub) {
    pec = new famouso::config::PEC(famouso::mw::Subject(sub));
}

PublisherEC::~PublisherEC() {
    delete pec;
}

int PublisherEC::announce() {
    pec->announce();
    return 1;
}

int PublisherEC::unannounce() {
    // needs not to be implemented
    // unannouncement takes place if channel is destructed
    return 1;
}

int PublisherEC::publish(const event_t &event) {
    famouso::mw::Event e(famouso::mw::Subject(event.subject));
    e.length = event.len;
    e.data = reinterpret_cast<uint8_t *>(event.data);
    pec->publish(e);
    return 1;
}

init::init(){
    famouso::init<famouso::config>();
}
init::~init(){}

