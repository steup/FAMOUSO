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

template <class A, class B>
class NA;


// einfacher ANL
template <class NL>
class ANL {
        typedef NA<ANL, ANL> _naT;
        _naT &_na;
        NL nl;
    public:
        ANL(_naT &n): _na(n), nl(*this) {}

        void announce() {
            DEBUG(("%s 0x%x\n", __PRETTY_FUNCTION__ , this));
            nl.bind();
        }
        void call() {
            DEBUG(("%s 0x%x\n", __PRETTY_FUNCTION__ , this));
            _na.call();
        }

};

class NLCAN {
        typedef ANL<NLCAN> ANLCAN;
        ANLCAN &_anl;
    public:
        NLCAN(ANLCAN &a) : _anl(a) {}

        void bind() {
            DEBUG(("%s 0x%x, sizeof(%d)\n", __PRETTY_FUNCTION__ , this, sizeof(NLCAN)));
            _anl.call();
        }
};

class NLUDP {
        typedef ANL<NLUDP> ANLUDP;
        ANLUDP &_anl;
    public:
        NLUDP(ANLUDP &a) : _anl(a) {}

        void bind() {
            DEBUG(("%s 0x%x\n", __PRETTY_FUNCTION__ , this));
            _anl.call();
        }
};

template <class A, class B>
class NA {
        A _anl1;
        B _anl2;
    public:
        NA(): _anl1(*this), _anl2(*this) {}
        void announce() {
            DEBUG(("%s\n", __PRETTY_FUNCTION__));
            _anl1.announce();
            _anl2.announce();

        };
        void call() {
            DEBUG(("%s 0x%x\n", __PRETTY_FUNCTION__ , this));
        }
};


class config {
    public:
        typedef ANL<NLCAN > ANL_CAN0;
        typedef ANL<NLCAN > ANL_CAN1;
        typedef ANL<NLUDP > ANL_UDP;
        typedef NA<ANL_CAN0, ANL_CAN1 > GWLayerCAN;
        typedef NA<ANL_UDP, GWLayerCAN > GWLayer;


        typedef GWLayerCAN Layer;
};



// und einige einfache Instanzierungen zur Probe
int main(int argc, char **argv) {
    config::Layer testCAN;
    testCAN.announce();
}
