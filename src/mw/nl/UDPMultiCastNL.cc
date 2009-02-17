#include "mw/nl/UDPMultiCastNL.h"
#include "util/endianness.h"

namespace famouso{
	namespace mw{
		namespace nl{

	void UDPMultiCastNL::init() {
		m_socket.open( m_endpoint_listen.protocol());
		m_socket.set_option( boost::asio::ip::udp::socket::reuse_address(true) );
		// m_socket.set_option( boost::boost::asio::ip::multicast::hops( 3 ) );
		// m_socket.set_option( boost::boost::asio::ip::multicast::enable_loopback(false) );
		m_socket.set_option( boost::asio::ip::multicast::enable_loopback( true ) );

		m_socket.bind( m_endpoint_listen );

		m_socket.async_receive_from(
			boost::asio::buffer(m_buffer, info::payload), m_endpoint_from,
			boost::bind( &UDPMultiCastNL::interrupt, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);
	}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void UDPMultiCastNL::bind( const famouso::mw::Subject &s, SNN &snn ){
		//  private Multicast Adressen: 239.0.0.0 - 239.255.255.255
		// 8 Byte -> 3 Byte
		 std::stringstream addr;

		uint8_t temp_ = s.tab[5];
		if( temp_ == 0 ) temp_++;
		if(temp_ == 255) temp_--;

		addr << "239." << static_cast<short>( s.tab[0] ) << "." << static_cast<short>( s.tab[7] )<< "." << static_cast<short>( temp_ );
		snn = boost::asio::ip::address::from_string( addr.str() );
		m_socket.set_option( boost::asio::ip::multicast::join_group( snn ) );
	}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void UDPMultiCastNL::interrupt( const boost::system::error_code& error, size_t bytes_recvd ){
		if (!error)
		{
			m_incoming_packet.snn = SNN( m_endpoint_from.address() );
			m_incoming_packet.s.value = *(reinterpret_cast<uint64_t*>( m_buffer ) ) ;
			m_incoming_packet.data = m_buffer + sizeof( famouso::mw::Subject );
			m_incoming_packet.data_length = bytes_recvd-sizeof( famouso::mw::Subject );
			// ready to fetch!
				Packet_t p;
				fetch( p );

		}else{
			std::cerr << "while receiving : " << error.message() << std::endl;
		}


		m_socket.async_receive_from(
			boost::asio::buffer(m_buffer, info::payload), m_endpoint_from,
			boost::bind( &UDPMultiCastNL::interrupt, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);

	}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void UDPMultiCastNL::fetch(Packet_t& p) {
		p.s = m_incoming_packet.s;
		p.snn = m_incoming_packet.snn;
		p.data = m_incoming_packet.data; // zeigt auf m_buffer
		p.data_length = m_incoming_packet.data_length;

	//	std::cout << p.snn.to_string() << " (Subject #" << p.s.value <<") < ";
	//	std::cout.write( reinterpret_cast<const char*>(p.data), p.data_length );
	//	std::cout << std::endl;
	}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void UDPMultiCastNL::deliver(const Packet_t& p) {
		std::vector<boost::asio::const_buffer> buffers;
		buffers.push_back(boost::asio::buffer( &p.s, sizeof( famouso::mw::Subject  )));
		buffers.push_back(boost::asio::buffer( p.data, p.data_length ));

		m_socket.async_send_to(
			buffers,
			boost::asio::ip::udp::endpoint( p.snn, port ),
			boost::bind(
				&UDPMultiCastNL::handle_send_to,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);
	}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

	void UDPMultiCastNL::handle_send_to( const boost::system::error_code& error, size_t bytes_recvd ){
		if (!error)
		{

		}else{
			std::cerr << "while sending : " << error.message() << std::endl;
		}
	}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	UDPMultiCastNL::SNN UDPMultiCastNL::lastPacketSNN(){
		return m_incoming_packet.snn;
	}

}}} // namespaces
