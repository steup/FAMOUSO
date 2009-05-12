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

#ifndef __UID_h__
#define __UID_h__

#include <stdint.h>
#include "util/endianness.h"

/*!
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

    bool operator < (const UID &a) const {
        return _value < a._value;
    }

    bool operator == (const UID &a) const {
        return _value == a._value;
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
        _value = *reinterpret_cast<const uint64_t*>(uid);
    }
};

#endif /* __UID_h__ */

