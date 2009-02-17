#ifndef __UID_h__
#define __UID_h__

#include <stdint.h>

/*! \file uid.h
 *  \class UID
 *  \brief The uid describes an unique identifier.
 *
 *  The UID is an 64Bit unique identifier which is used for the explicit
 *  labeling of events and event channels. It represents the subject or
 *  topic of the respective entity. The value is an eight byte character
 *  array and it is interpreted thus too. This enables handling in an
 *  machine independent way because we do not need to consider different
 *  byte orders.
 *
 */
struct UID {

    union {
        uint64_t value;
        uint8_t tab[8];
    };

    UID() : value(0) {}

    UID(uint64_t uid) : value(uid) {}

    UID(const uint8_t *uid) {
        do_init(reinterpret_cast<const uint8_t*>(uid));
    }

    UID(const char *uid) {
        do_init(reinterpret_cast<const uint8_t*>(uid));
    }

    bool operator == (const UID &a) const{
        return value==a.value;
    }

private:
    void do_init(const uint8_t *uid) {
            value= *reinterpret_cast<const uint64_t*>(uid);
    }

};

#endif /* __UID_h__ */

