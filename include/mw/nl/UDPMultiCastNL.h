#ifndef __UDPMultiCastNL_hpp__
#define __UDPMultiCastNL_hpp__

#include <stdio.h>
#include <asio.hpp>
#include <asio/error.hpp>
#include <boost/bind.hpp>
#include <boost/pool/detail/singleton.hpp>
#include <boost/utility.hpp>
#include <string>
#include "mw/nl/Packet.hpp"
#include "util/ios.h"


namespace famouso {

class UDPMultiCastNL : boost::noncopyable {
	public:
		
		struct info{
			enum {
				payload=8192-sizeof( Subject),
			};
		};
		
		// type of an address
		typedef asio::ip::address SNN;
		
		// type of a packet
		typedef Packet<SNN> Packet_t;
		
		/**
		 * @brief default constructor
		 *
		 * Sets local variables and calls init.
		 * 
		 * @param p port (default is 9999)
		 */
		UDPMultiCastNL( int p = 9999 )
		:socket_( famouso::ios::instance() ), port( p ),
		endpoint_listen( asio::ip::address::from_string("0.0.0.0"), port )
		{
			init();
		}
		

		
		/**
		 * @brief destructor
		 *
		 *  Closes the socket.
		 *
		 *
		 */
		~UDPMultiCastNL() {
			socket_.close( );
		}

		/**
		 * Sets the options for the socket and starts receiving.
		 */
		void init();
		
		/**
		 * @brief subscribe a subject
		 * 
		 * Start listening on the multicast-address that belongs to the subject.
		 * 
		 * @param s subject
		 * @param snn bound address
		 */
		void bind( const Subject &s, SNN &snn );

		/**
		 * @brief called by deliver
		 *
		 * Is called when sending is finished.
		 * @param error tells if sending was successful
		 * @param bytes_recvd the amount of bytes that were sent
		 */	
		void handle_send_to( const asio::error_code& error, size_t bytes_recvd );
		
		/**
		 * @brief publish
		 *
		 * Sends the given packet.
		 *
		 * @param p packet
		 */
		void deliver( const Packet_t& p );
		void deliver_fragment( const Packet_t& p){ deliver( p ); };
		
		/**
		 * @brief processes incoming packets
		 * 
		 * @param p fetched packet is saved here
		 */
		void fetch( Packet_t& p);
		
		/**
		 * @brief handle called on receive
		 * 
		 * Will be called, whenever a packet was received.
		 *
		 */
		void interrupt( const asio::error_code& error, size_t bytes_recvd );
			
	private:
		const int port;
		asio::ip::udp::socket socket_;
		asio::ip::udp::endpoint endpoint_listen;
		asio::ip::udp::endpoint endpoint_from;
		uint8_t buffer_[sizeof( Subject )+info::payload]; // Subject and Payload have to be sent
		Packet_t incoming_packet;
};

}


#endif /* __UDPMultiCastNL_hpp__ */
