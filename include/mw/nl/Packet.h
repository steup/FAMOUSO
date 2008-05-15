#ifndef __Packet_h__
#define __Packet_h__

#include <stdint.h>

template<class T>
struct Packet {
  const T& snn;
  uint8_t* data;
};


#endif /* __Packet_h__ */
