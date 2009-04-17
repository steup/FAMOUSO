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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DEBUG(X) printf X

typedef uint64_t Subject;

char *deviceUDP = "eth0";
char *deviceCAN0 = "can0";
char *deviceCAN1 = "can1";

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
        typedef uint16_t SNN; // CAN_ETAG
};

//einfacher NL fuer UDP
template < char *&_dev >
class NLUDP {
        uint32_t ip;
    public:
        NLUDP() : ip(55) {
        }

        void bind() {
            DEBUG(("%s %s\n", __PRETTY_FUNCTION__, _dev));
        }
        typedef uint32_t SNN; // UDP_ETAG
};

// einfacher ANL
template < class NL >
class ANL {
        NL nl;
    public:

        void announce() {
            DEBUG(("%s\n", __PRETTY_FUNCTION__));
            nl.bind();
        }
        typedef typename NL::SNN SNN;
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
    private:
        static T _na;
        struct Topic {
            Subject subj;
            typename T::SNN SNN;
        } topic;
};
template < class T > T EC < T >::_na;

// einfach Definition eines EventChannels
template < class T > void EC < T >::announce() {
    DEBUG(("%s sizeof(SNN)=%d\n", __PRETTY_FUNCTION__, sizeof(topic.SNN)));
    _na.announce();
};

// einige Typendefinitionen, die den Umgang mit Templateparametern
// bezueglich Device-Spezifikation zeigen
typedef ANL < NLUDP < deviceUDP > >ANL_UDP;
typedef ANL < NLCAN < deviceCAN0 > >ANL_CAN0;
typedef ANL < NLCAN < deviceCAN1 > >ANL_CAN1;

typedef EC < ANL < NLCAN < deviceCAN0 > > >avrEC;

class config {
    public:
        typedef NLUDP < deviceUDP > NL_UDP;
        typedef NLCAN < deviceCAN0 > NL_CAN0;
        typedef NLCAN < deviceCAN1 > NL_CAN1;
        typedef ANL < NL_UDP > ANL_UDP;
        typedef ANL < NL_CAN0 > ANL_CAN0;
        typedef ANL < NL_CAN1 > ANL_CAN1;

        typedef NA < ANL_CAN0, ANL_CAN1 > GW_CAN;
        typedef NA < ANL_UDP, GW_CAN > GW_UDP_CANs;
        typedef ECH_A < GW_UDP_CANs > ECH_SINGLE;
        typedef EC < ECH_SINGLE > EC_BASE;
};

// und einige einfache Instanzierungen zur Probe
int main(int argc, char **argv) {

    DEBUG(("Ausfuehrliches Beispiel fuer z.B. ein Gateway mit drei Netzwerken\n"));
    config::EC_BASE a;
    a.announce();

    DEBUG(("\nBeispiel fuer minimales System z.B. fuer den AVR\n"));
    avrEC avr, avr2;
    avr.announce();
    avr2.announce();

    DEBUG(("sizeof(avrEC)=%d sizeof(config::EC_BASE)=%d\n", sizeof(avrEC), sizeof(config::EC_BASE)));
}
