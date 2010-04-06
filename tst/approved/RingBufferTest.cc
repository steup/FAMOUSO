/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
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


#include "object/RingBuffer.h"
#include "boost/circular_buffer.hpp"

#include "boost/random/linear_congruential.hpp"

#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


object::RingBuffer<unsigned char, 5> my;
boost::circular_buffer<unsigned char> their(5);

void check() {
    FAMOUSO_ASSERT(my.get_count() == their.size());
    for (int i = 0; i < my.get_count(); i++) {
        unsigned char m = my[i], t = their[i];
        if (m != t) {
            printf("\n\nError: %i != %i at position %i\n", (int)my[i], (int)their[i], i);
            abort();
        }
        printf("%i ", (int)my[i]);
    }
    printf("\n");
    if (!my.is_empty() && my.front() != their.front()) {
        printf("\n\nError: %i != %i (front)\n", (int)my.front(), (int)their.front());
        abort();
    }
    if (!my.is_empty() && my.back() != their.back()) {
        printf("\n\nError: %i != %i (back)\n", (int)my.back(), (int)their.back());
        abort();
    }
}

boost::rand48 generator(time(0));

unsigned char rand_byte(int max) {
    return generator() % (max + 1);
}

int main() {


    printf("RingBuffer test\n\n");

    while (1) {
        // Run random action
        const char * action;
        while (1) {
            unsigned int t = rand_byte(6);
            if (t == 0 && !my.is_full()) {
                action = "push_front";
                t = rand_byte(255);
                my.push_front(t);
                their.push_front(t);
                break;
            }
            if (t == 1 && !my.is_full()) {
                action = "push_back";
                t = rand_byte(255);
                my.push_back(t);
                their.push_back(t);
                break;
            }
            if (t == 2 && !my.is_empty()) {
                action = "pop_front";
                my.pop_front();
                their.pop_front();
                break;
            }
            if (t == 3 && !my.is_empty()) {
                action = "pop_back";
                t = rand_byte(255);
                my.pop_back();
                their.pop_back();
                break;
            }
            if (t == 4 && !my.is_empty()) {
                action = "back";
                t = rand_byte(255);
                my.back() = t;
                their.back() = t;
                break;
            }
            if (t == 5 && !my.is_empty()) {
                action = "front";
                t = rand_byte(255);
                my.front() = t;
                their.front() = t;
                break;
            }
            if (t == 6 && !my.is_empty()) {
                action = "[]";
                t = rand_byte(255);
                int i = rand_byte(my.get_count()-1);
                my[i] = t;
                their[i] = t;
                break;
            }
        }
        printf("Action: %s\n", action);

        // Check if my and their content is equal
        check();
        usleep(10000);
    }

    return 0;
}



