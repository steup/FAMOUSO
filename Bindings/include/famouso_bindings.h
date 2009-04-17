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

#endif
