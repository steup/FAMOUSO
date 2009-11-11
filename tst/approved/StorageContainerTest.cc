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

// for at90can128 with 16MHz CPU clock
#define CPU_FREQUENCY 16000000
#include "debug.h"

#include <stddef.h>

struct testStruct {
    int  test;
    void foo() {
        logging::log::emit() << "testStruct::foo() " << test << logging::log::endl;
    }
    testStruct() : test(0) {
        logging::log::emit() << "testStruct::testStruct() " << test << logging::log::endl;
    }
    testStruct(int i) : test(i) {
        logging::log::emit() << "testStruct::testStruct(int) " << test << logging::log::endl;
    }
    ~testStruct() {
        logging::log::emit() << "testStruct::~testStruct() " << test << logging::log::endl;
    }
    bool operator == (const testStruct& t) {
        return !!(this == &t);
    }

    /*! \brief placement new operator to allow a constructor call
     *         on pre-allocated memory
     */
    void *operator new(size_t size, void* buffer) {
        return buffer;
    }
};

template < typename T, int N = 12 >
struct testContainer {
    testContainer () {
        {
            logging::log::emit() << "test program starts" << logging::log::endl;
            typedef T SS;
            SS s;
            int t = 0;
            typename SS::pointer b;
            logging::log::emit() << "reserve objects " << logging::log::endl;
            while ((t < N) && (b = s.newElement()))
                new(b) testStruct(t++);

            logging::log::emit() << "traverse object container " << logging::log::endl;
            typename SS::iterator i = s.begin();
            while (i != s.end()) {
                (*i).foo();
                ++i;
            }

            logging::log::emit() << "clean some objects from object container " << logging::log::endl;
            i = s.begin();
            ++i;
            ++i;
            s.delElement(&*i);
            i = s.begin();
            ++i;
            ++i;
            ++i;
            ++i;
            ++i;
            s.delElement(&*i);

            logging::log::emit() << "destruct object container" << logging::log::endl;
        }
        logging::log::emit() << "test program finished" << logging::log::endl;
    }
};

#include "object/Storage.h"
int main(int argc, char **argv) {
    testContainer< object::Storage<testStruct>::depot >();
}

