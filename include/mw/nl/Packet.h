#ifndef __Packet_h__
#define __Packet_h__

#include <stdint.h>
#include "mw/common/Subject.h"

template<class T>
struct Packet {
	T snn; // IP-address
	famouso::mw::Subject s; // Subject
	uint8_t* data; // pointer to data
	uint16_t data_length; // size of data

	Packet(){};

	Packet( const Packet<T>& p )
	: snn( p.snn ), s( p.s ), data( p.data ), data_length( p.data_length ){}

	Packet( T address, famouso::mw::Subject subject, uint8_t d, uint16_t length  )
	: snn( address ), s( subject ), data( d ), data_length( length ){}

	Packet(T address, uint8_t *d, uint16_t length  )
	: snn( address ), s(0), data( d ), data_length( length ){}
};



#endif /* __Packet_h__ */
