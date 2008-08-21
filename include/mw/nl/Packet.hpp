#ifndef __Packet_hpp__
#define __Packet_hpp__

#include <stdint.h>
#include "mw/common/Subject.h"

template<class T>
struct Packet {
	T snn; // IP-address
	Subject s; // Subject
	uint8_t* data; // pointer to data
	uint16_t data_length; // size of data
	
	Packet(){};

	Packet( const Packet<T>& p )	
	: snn( p.snn ), s( p.s ), data( p.data ), data_length( p.data_length ){}	
		
	Packet( T address, Subject subject, uint8_t d, uint16_t length  )
	: snn( address ), s( subject ), data( d ), data_length( length ){}	
};


#endif /* __Packet_hpp__ */
