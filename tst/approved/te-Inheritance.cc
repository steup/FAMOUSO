#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "case/Delegate.h"

#define DEBUG(X) printf X

using namespace std;

char *deviceUDP = "eth0";
char *deviceCAN0 = "can0";
char *deviceCAN1 = "can1";

class BaseNL {
	public:
		const BaseNL* id() {return this;} 
};

// Callback zum EventLayer
famouso::util::Delegate<BaseNL*> EL_CallBack;

// einfacher NL fuer CAN
template < char *&_dev >
class NLCAN : public BaseNL{
    uint8_t i;
public:
    NLCAN ():i (0) {
    }

    void bind () {
        DEBUG (("%s %d %s this=0x%x\n", __PRETTY_FUNCTION__, i, _dev,
                reinterpret_cast < uint32_t > (this)));
		EL_CallBack(const_cast<BaseNL*>(id()));
    }
	void fetch(BaseNL* bnl) {
		DEBUG(("%s %s 0x%x\n", __PRETTY_FUNCTION__, _dev, bnl));
	}
};

//einfacher NL fuer UDP
template < char *&_dev >
class NLUDP : public BaseNL{
public:
    NLUDP () {
	}
	void bind (){
        DEBUG (("%s %s this=0x%x\n", __PRETTY_FUNCTION__, _dev,
                reinterpret_cast < uint32_t > (this)));
		EL_CallBack(this);
    }
	void fetch(BaseNL* bnl) {
		DEBUG(("%s %s 0x%x\n", __PRETTY_FUNCTION__, _dev, bnl));
	};
};

// einfacher ANL
template < class NL >
class ANL:public NL {
public:

    void announce () {
        DEBUG (("%s\n", __PRETTY_FUNCTION__));
        NL::bind ();
    }
	void fetch(BaseNL* bnl) {
		DEBUG(("%s 0x%x\n", __PRETTY_FUNCTION__, bnl));
		NL::fetch(bnl);
	}
};

// NetworkAdapter fuer Gateways mit zwei oder mehr NetworkCards
// nimmt als Parameter ANLs sowie auch Networkadapter selbst
template < class A, class B >
class NA : public A, public B {
public:
    void announce () {
        DEBUG (("%s\n", __PRETTY_FUNCTION__));
		DEBUG (("rechts 0x%x \t links 0x%x\n",A::id(), B::id()));
        A::announce ();
        B::announce ();
    };
	const BaseNL* id () { return reinterpret_cast< BaseNL*>(0);}

	void fetch(BaseNL* bnl) {
		DEBUG(("%s 0x%x\n", __PRETTY_FUNCTION__, bnl));
		if ((A::id()==0) || (A::id() == bnl))
			A::fetch(bnl);
		else {
			if ((B::id() == 0) || (B::id() == bnl))
				B::fetch(bnl);
		}
	}
};

// Adapter zum EventChannelHandler
template < class T >
class ECH_A : public T {
public:
	void init () {
		EL_CallBack.bind<ECH_A<T>,&ECH_A<T>::fetch>(this);
	}

    void announce () {
        DEBUG (("%s\n", __PRETTY_FUNCTION__));
        T::announce ();
    };

	void fetch(BaseNL* bnl) {
		T::fetch(bnl);
	}
};

// Adapter zum EventChannelHandler
template < class T >
class EC {
    static T _na;
public:
	static T& el() { return _na;}
    void announce ();
};

template < class T > T EC < T >::_na;

// einfach Definition eines EventChannels
template < class T >
void EC < T >::announce () {
    DEBUG (("%s\n", __PRETTY_FUNCTION__));
    _na.announce ();
};

namespace famouso {
namespace mw {
namespace el {
template <class T>
inline void init () {
	T::el().init();
}
}
}
}

// einige Typendefinitionen, die den Umgang mit Templateparametern
// bezueglich Device-Spezifikation zeigen
typedef ANL < NLUDP < deviceUDP > >ANL_UDP;
typedef ANL < NLCAN < deviceCAN0 > >ANL_CAN0;
typedef ANL < NLCAN < deviceCAN1 > >ANL_CAN1;

typedef EC < ECH_A < ANL < NLCAN < deviceCAN0 > > > >avrEC;
typedef EC < ECH_A < NA < ANL_UDP, NA < ANL_CAN0, ANL_CAN1 > > > > ec_s;

// und einige einfache Instanzierungen zur Probe
int main (int argc, char **argv) {

	DEBUG(("size of a delegate callback %d\n",sizeof(EL_CallBack)));
	// richtige Initialisierung von FAMOUSO
	famouso::mw::el::init<ec_s>();
    DEBUG (("Ausfuehrliches Beispiel fuer z.B. ein Gateway mit drei Netzwerken\n"));
    ec_s a;
    a.announce ();

	DEBUG(("\n\n"));
	EL_CallBack(0);
    DEBUG (("\nBeispiel fuer minimales System z.B. fuer den AVR\n"));
	// richtige Initialisierung von FAMOUSO
	famouso::mw::el::init<avrEC>();
    avrEC avr, avr2;
    avr.announce ();
    avr2.announce ();
}
