#include "mw/nl/UDPMultiCastNL.h"
#include "util/endianess.h"

namespace famouso{

	void UDPMultiCastNL::init() {
		socket_.open( endpoint_listen.protocol());
		socket_.set_option( asio::ip::udp::socket::reuse_address(true) );
		// socket_.set_option( boost::asio::ip::multicast::hops( 3 ) );
		// socket_.set_option( boost::asio::ip::multicast::enable_loopback(false) );
		socket_.set_option( asio::ip::multicast::enable_loopback( true ) );
		
		socket_.bind( endpoint_listen );

		socket_.async_receive_from(
			asio::buffer(buffer_, info::payload), endpoint_from,
			boost::bind( &UDPMultiCastNL::interrupt, this,
				asio::placeholders::error,
				asio::placeholders::bytes_transferred
			) 
		);
	}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------	
	
	void UDPMultiCastNL::bind( const Subject &s, SNN &snn ){
		//  private Multicast Adressen: 239.0.0.0 - 239.255.255.255
		// 8 Byte -> 3 Byte
		 std::stringstream addr;
		Subject s_temp = htonll( s.value );
		
		uint8_t temp_ = s_temp.tab[5];
		if( temp_ == 0 ) temp_++;
		if(temp_ == 255) temp_--;
		
		addr << "239." << static_cast<short>( s_temp.tab[0] ) << "." << static_cast<short>( s_temp.tab[7] )<< "." << static_cast<short>( temp_ );
		snn = asio::ip::address::from_string( addr.str() );
		socket_.set_option( asio::ip::multicast::join_group( snn ) );
	}
	
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------	
	
	void UDPMultiCastNL::interrupt( const asio::error_code& error, size_t bytes_recvd ){
		if (!error)
		{		
			incoming_packet.snn = SNN( endpoint_from.address() ); 
			incoming_packet.s.value = ntohll( *(reinterpret_cast<uint64_t*>( buffer_ ) ) );
			incoming_packet.data = buffer_ + sizeof( Subject );
			incoming_packet.data_length = bytes_recvd-sizeof( Subject );
			// ready to fetch!
				Packet_t p;
				fetch( p );
		
		}else{
			std::cerr << "while receiving : " << error.message() << std::endl;
		}
		
		
		socket_.async_receive_from(
			asio::buffer(buffer_, info::payload), endpoint_from,
			boost::bind( &UDPMultiCastNL::interrupt, this,
				asio::placeholders::error,
				asio::placeholders::bytes_transferred
			) 
		);
		
	}
	
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------	

	void UDPMultiCastNL::fetch(Packet_t& p) {
		p.s = incoming_packet.s;
		p.snn = incoming_packet.snn;
		p.data = incoming_packet.data; // zeigt auf buffer_
		p.data_length = incoming_packet.data_length;
		
		std::cout << p.snn.to_string() << " (Subject #" << p.s.value <<") < ";
		std::cout.write( reinterpret_cast<const char*>(p.data), p.data_length );
		std::cout << std::endl;
	}
	
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------	
		
	void UDPMultiCastNL::deliver(const Packet_t& p) {
		std::vector<asio::const_buffer> buffers;
		Subject s;
		s.value = htonll( p.s.value );
		buffers.push_back(asio::buffer( &s, sizeof( Subject )));
		buffers.push_back(asio::buffer( p.data, p.data_length ));
		
		socket_.async_send_to(
			buffers,
			asio::ip::udp::endpoint( p.snn, port ),
			boost::bind(
				&UDPMultiCastNL::handle_send_to, 
				this,
				asio::placeholders::error,
				asio::placeholders::bytes_transferred
			)
		);	
	}	
	
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------	

	void UDPMultiCastNL::handle_send_to( const asio::error_code& error, size_t bytes_recvd ){
		if (!error)
		{		

		}else{
			std::cerr << "while sending : " << error.message() << std::endl;
		}
	}

}
