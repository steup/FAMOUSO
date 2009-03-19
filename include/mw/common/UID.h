#ifndef __UID_h__
#define __UID_h__

#include <stdint.h>
#include "util/endianness.h"

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
private:
    union {
        uint64_t _value;
        uint8_t _tab[8];
    };

    public:
    explicit UID() : _value(0) {}

    explicit UID(uint64_t uid) : _value(htonll(uid)) {}

    explicit UID(const uint8_t *uid) {
        do_init(reinterpret_cast<const uint8_t*>(uid));
    }

    UID(const char *uid) {
        do_init(reinterpret_cast<const uint8_t*>(uid));
    }

    bool operator < (const UID &a) const{
        return _value < a.value();
    }

    bool operator == (const UID &a) const{
        return _value == a.value();
    }

    uint8_t* tab() {
        return _tab;
    }
    const uint8_t* tab() const {
        return _tab;
    }

    const uint64_t value() const {
        return ntohll(_value);
    }
private:
    void do_init(const uint8_t *uid) {
            _value= *reinterpret_cast<const uint64_t*>(uid);
    }

};

#endif /* __UID_h__ */

