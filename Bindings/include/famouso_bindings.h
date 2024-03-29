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

#ifndef __FAMOUSO_BINDINGS_H__
#define __FAMOUSO_BINDINGS_H__

#include "famouso_bindings_config.h"

typedef unsigned long long  subject_t;
//typedef uint64_t  subject_t;

typedef struct {
    subject_t subject;
    int len;
// uint32_t len;
    char *data;
} event_t;

class SubscriberEC {
        famouso::config::SEC *sec;
        void cb(famouso::mw::api::SECCallBackData &cb);
        uint32_t first, last, max;
        struct Storage {
            Storage() : used(0) {
                event.data = data;
            }
            uint8_t used;
            event_t event;
            char data[1<<16];
        } *events;

    public:
        SubscriberEC(const subject_t&, uint32_t count = 5);
        virtual ~SubscriberEC();
        int subscribe();
        int unsubscribe();
        int poll(event_t &e);
        virtual void notify();
};


class PublisherEC {
        famouso::config::PEC *pec;
    public:
        PublisherEC(const subject_t&);
        ~PublisherEC();
        int announce();
        int unannounce();
        int publish(const event_t&);
};

class init {
    public:
        init();
        ~init();
};

#endif
