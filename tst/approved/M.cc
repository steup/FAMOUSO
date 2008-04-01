#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// $Author$
// $Id$

#define DEBUG(X) printf X

typedef uint64_t Subject;

char *deviceCAN0 = "can0";

// einfacher NL fuer CAN
template < char *&_dev >
class NLCAN {
    uint8_t txnode;
public:
    NLCAN():txnode(0) {
    }

    void bind() {
        DEBUG(("%s %d %s this=0x%x\n", __PRETTY_FUNCTION__, txnode, _dev,
               this));
    }
//    struct SNN {
        typedef uint16_t SNN;	// CAN_ETAG
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
//	struct SNN{
		typedef typename NL::SNN SNN;
//	};
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
        typename A::SNN	A_SNN;
        typename B::SNN	B_SNN;
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
	typedef typename T::SNN	SNN;

};

// Adapter zum EventChannelHandler
template < class T >
class EC {
public:
    void announce();
private:
    static T _na;
	typename T::SNN	SNN;
	Subject	subj;
};
template < class T > T EC < T >::_na;

// einfach Definition eines EventChannels
template < class T > void EC < T >::announce() {
    DEBUG(("%s sizeof(SNN)=%d sizeof(T)=%d\n", __PRETTY_FUNCTION__, sizeof(SNN),sizeof(T)));
    _na.announce();
};

typedef ANL < NLCAN < deviceCAN0 >	>ANLCAN;
typedef EC < ECH_A<  ANLCAN > >avrEC1;
typedef EC < ECH_A< NA< ANLCAN, ANLCAN> > >avrEC;

// und einige einfache Instanzierungen zur Probe
int main(int argc, char **argv) {
	uint8_t data[argc];
    DEBUG(("sizeof(data)=%d\n",sizeof(data)));
    DEBUG(("\nBeispiel fuer minimales System z.B. fuer den AVR avrEC1=%d avrEC=%d\n",sizeof(avrEC1),sizeof(avrEC)));
    avrEC1 avr;
	avrEC avr2;
    avr.announce();
    avr2.announce();
}
