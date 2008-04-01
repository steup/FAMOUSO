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
  ANL(_naT &n):_na(n),nl(*this){}

  void announce(){
    DEBUG(("%s 0x%x\n",__PRETTY_FUNCTION__ , this ));
    nl.bind();
  }
  void call(){
    DEBUG(("%s 0x%x\n",__PRETTY_FUNCTION__ , this ));
    _na.call();
  }

};

class NLCAN {
  typedef ANL<NLCAN> ANLCAN;
  ANLCAN &_anl;
public:
  NLCAN(ANLCAN &a) : _anl(a) {}

  void bind(){
    DEBUG(("%s 0x%x, sizeof(%d)\n",__PRETTY_FUNCTION__ , this, sizeof(NLCAN) ));
    _anl.call();
  }
};

class NLUDP {
  typedef ANL<NLUDP> ANLUDP;
  ANLUDP &_anl;
public:
  NLUDP(ANLUDP &a) : _anl(a) {}

  void bind(){
    DEBUG(("%s 0x%x\n",__PRETTY_FUNCTION__ , this ));
    _anl.call();
  }
};

template <class A, class B>
class NA {
  A _anl1;
  B _anl2;
public:
  NA(): _anl1(*this),_anl2(*this) {}
  void announce(){
    DEBUG(("%s\n",__PRETTY_FUNCTION__  ));
    _anl1.announce();
    _anl2.announce();

  };
  void call(){
    DEBUG(("%s 0x%x\n",__PRETTY_FUNCTION__ , this ));
  }
};


class config{
public:
  typedef ANL<NLCAN > ANL_CAN0;
  typedef ANL<NLCAN > ANL_CAN1;
  typedef ANL<NLUDP > ANL_UDP;
  typedef NA<ANL_CAN0, ANL_CAN1 > GWLayerCAN;
  typedef NA<ANL_UDP, GWLayerCAN > GWLayer;
  

  typedef GWLayerCAN Layer;
};



// und einige einfache Instanzierungen zur Probe
int main(int argc, char **argv){
  config::Layer testCAN;
  testCAN.announce();
}
