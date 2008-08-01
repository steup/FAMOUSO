#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DEBUG(X) printf X

using namespace std;

char *deviceUDP = "eth0";
char *deviceCAN0 = "can0";
char *deviceCAN1 = "can1";

// einfacher NL fuer CAN
template < char *&_dev >
class NLCAN {
    uint8_t i;
public:
    NLCAN ():i (0) {
    }

    virtual ~ NLCAN () {
    }
    void bind () {
        DEBUG (("%s %d %s this=0x%x\n", __PRETTY_FUNCTION__, i, _dev,
                reinterpret_cast < uint32_t > (this)));
    }
    virtual void callback () {
    }
};

//einfacher NL fuer UDP
template < char *&_dev >
class NLUDP {
public:
    NLUDP () {
	} 
	void bind (){
        DEBUG (("%s %s\n", __PRETTY_FUNCTION__, _dev));
    }
};

// einfacher ANL
template < class NL >
class ANL:public NL {
public:

    void announce () {
        DEBUG (("%s\n", __PRETTY_FUNCTION__));
        NL::bind ();
    }
};

// NetworkAdapter fuer Gateways mit zwei oder mehr NetworkCards
// nimmt als Parameter ANLs sowie auch Networkadapter selbst
template < class A, class B >
class NA : public A, public B {
public:
    void announce () {
        DEBUG (("%s\n", __PRETTY_FUNCTION__));
        A::announce ();
        B::announce ();

    };
};

// Adapter zum EventChannelHandler
template < class T >
class ECH_A : public T {
public:
    void announce () {
        DEBUG (("%s\n", __PRETTY_FUNCTION__));
        T::announce ();
    };
};

// Adapter zum EventChannelHandler
template < class T >
class EC {
    static T _na;
public:
    void announce ();
};

template < class T > T EC < T >::_na;

// einfach Definition eines EventChannels
template < class T >
void EC < T >::announce () {
    DEBUG (("%s\n", __PRETTY_FUNCTION__));
    _na.announce ();
};

// einige Typendefinitionen, die den Umgang mit Templateparametern
// bezueglich Device-Spezifikation zeigen
typedef ANL < NLUDP < deviceUDP > >ANL_UDP;
typedef ANL < NLCAN < deviceCAN0 > >ANL_CAN0;
typedef ANL < NLCAN < deviceCAN1 > >ANL_CAN1;

typedef EC < ANL < NLCAN < deviceCAN0 > > >avrEC;

// und einige einfache Instanzierungen zur Probe
int main (int argc, char **argv) {

    DEBUG (("Ausfuehrliches Beispiel fuer z.B. ein Gateway mit drei Netzwerken\n"));
    EC < ECH_A < NA < ANL_UDP, NA < ANL_CAN0, ANL_CAN1 > > > >a;
    a.announce ();

    DEBUG (("\nBeispiel fuer minimales System z.B. fuer den AVR\n"));
    avrEC avr, avr2;
    avr.announce ();
    avr2.announce ();
}
