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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DEBUG(X) printf X

typedef uint64_t Subject;

char *deviceCAN0 = "can0";

// einfacher NL fuer CAN
template < char *&_dev >
class NLCAN {
        uint8_t txnode;
    public:
        NLCAN(): txnode(0) {
        }

        void bind() {
            DEBUG(("%s %d %s this=0x%x\n", __PRETTY_FUNCTION__, txnode, _dev,
                   this));
        }
//    struct SNN {
        typedef uint16_t SNN; // CAN_ETAG
//    };
};

// einfacher ANL
template < class NL >
class ANL {
        NL nl;
    public:

        void announce() {
            DEBUG(("%s sizeof(SNN)=%d\n", __PRETTY_FUNCTION__, sizeof(SNN)));
            nl.bind();
        }
// struct SNN{
        typedef typename NL::SNN SNN;
// };
//
};

// NetworkAdapter fuer Gateways mit zwei oder mehr NetworkCards
// nimmt als Parameter ANLs sowie auch Networkadapter selbst
template < class A, class B >
class NA {
        A _anl1;
        B _anl2;
    public:

        struct SNN {
            typename A::SNN A_SNN;
            typename B::SNN B_SNN;
        };

        void announce() {
            DEBUG(("%s sizeof(SNN)=%d\n", __PRETTY_FUNCTION__, sizeof(SNN)));
            _anl1.announce();
            _anl2.announce();
        };

};

// Adapter zum EventChannelHandler
template < class T >
class ECH_A {
        T _na;
    public:
        void announce() {
            DEBUG(("%s sizeof(SNN)=%d\n", __PRETTY_FUNCTION__, sizeof(SNN)));
            _na.announce();
        };
        typedef typename T::SNN SNN;

};

// Adapter zum EventChannelHandler
template < class T >
class EC {
    public:
        void announce();
        static T _na;
        typename T::SNN SNN;
        Subject subj;
    private:
};
template < class T > T EC < T >::_na;

// einfach Definition eines EventChannels
template < class T > void EC < T >::announce() {
    DEBUG(("%s sizeof(SNN)=%d sizeof(T)=%d\n", __PRETTY_FUNCTION__, sizeof(SNN), sizeof(T)));
    _na.announce();
};

typedef ANL < NLCAN < deviceCAN0 > >ANLCAN;
typedef EC < ECH_A<  ANLCAN > >avrEC1;
typedef EC < ECH_A< NA< ANLCAN, ANLCAN> > >avrEC;

// und einige einfache Instanzierungen zur Probe
int main(int argc, char **argv) {
    uint8_t data[argc];
    DEBUG(("sizeof(data)=%d\n", sizeof(data)));
    DEBUG(("\nBeispiel fuer minimales System z.B. fuer den AVR avrEC1=%d avrEC=%d\n", sizeof(avrEC1), sizeof(avrEC)));
    avrEC1 avr;
    avrEC avr2;
    avr.announce();
    avr2.announce();
    avr.SNN = 1000;
    avr2.SNN.A_SNN = 10;
    avr2.SNN.B_SNN = 20;

    DEBUG(("%p %p %d\n", &avr2.SNN.A_SNN, &avr2.SNN.B_SNN, sizeof(avr2.SNN)));
}
