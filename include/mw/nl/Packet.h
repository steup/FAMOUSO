#ifndef __Packet_h__
#define __Packet_h__

#include <stdint.h>
#include "mw/common/Subject.h"

template<class T>
struct Packet {
	T snn; // short network name representation
	uint8_t* data; // pointer to data
	uint16_t data_length; // size of data

	Packet(){};

	Packet( const Packet<T>& p )
	: snn( p.snn ), data( p.data ), data_length( p.data_length ){}

	Packet( T address, uint8_t *d, uint16_t length  )
	: snn( address ), data( d ), data_length( length ){}
};



#endif /* __Packet_h__ */
