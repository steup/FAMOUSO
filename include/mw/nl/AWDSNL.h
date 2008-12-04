#ifndef __AWDSNL_hpp__
#define __AWDSNL_hpp__

#include <stdio.h>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <string>
#include "mw/nl/BaseNL.h"
#include "mw/nl/Packet.h"
#include "util/ios.h"
#include "mw/common/Subject.h"

#include "mw/el/EventLayerCallBack.h"

namespace famouso {
	namespace mw {
		namespace nl {

struct __attribute__((packed)) AWDS_Packet {
    struct constants {
        struct packet_size{
            enum {
                payload=1400
            };
        };
        struct packet_type {
            enum {
                publish_fragment = 0x7B,
                publish = 0x7C,
                subscribe=0x7D
            };
        };
    };
    struct __attribute__((packed)) Header {
        uint8_t addr[6];
        uint8_t type;
    private:
        uint8_t __pad;
    public:
        uint16_t size;
    };

    Header   header;
    uint8_t  data[constants::packet_size::payload];
};

template<uint16_t Port=8555>
class AWDSNL : public BaseNL, boost::noncopyable {
	public:

		struct info{
			enum {
				payload=AWDS_Packet::constants::packet_size::payload-sizeof( famouso::mw::Subject ),
			};
		};

		// type of an address
		typedef famouso::mw::Subject SNN;

		// type of a packet
		typedef Packet<SNN> Packet_t;

		/**
		 * @brief default constructor
		 */
		AWDSNL() :m_socket( famouso::ios::instance() ){}

		/**
		 * @brief destructor
		 *
		 *  Closes the socket.
		 *
		 */
		~AWDSNL() {
			m_socket.close( );
		}

		/**
		 * Sets the options for the socket and starts receiving.
		 */
		void init() {
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), Port);
            boost::system::error_code ec;
            m_socket.connect(endpoint, ec);
            if (ec) {
               // An error occurred.
                std::cerr << "Error connecting to awds-stub" << std::endl;
                exit(-1);
            }

            m_socket.async_receive(
                boost::asio::buffer(&awds_packet, sizeof(AWDS_Packet)),
                boost::bind( &AWDSNL::interrupt, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)
		);
      }

		/**
		 * @brief subscribe a subject
		 *
		 * @param s subject
		 * @param snn bound address
		 */
		void bind( const famouso::mw::Subject &s, SNN &snn ){
            snn=s;
         }

		/**
		 * @brief publish
		 *
		 * Sends the given packet.
		 *
		 * @param p packet
         *
         * \todo momentan nur broadcast. siehe spezification fuer andere Verfahren unter doc/psawds/ipc-protocol.txt
		 */
		void deliver( const Packet_t& p, uint8_t type= AWDS_Packet::constants::packet_type::publish){
            std::vector<boost::asio::const_buffer> buffers;
            AWDS_Packet::Header awds_header;
            awds_header.addr[0]=0xff;
            awds_header.addr[1]=0xff;
            awds_header.addr[2]=0xff;
            awds_header.addr[3]=0xff;
            awds_header.addr[4]=0xff;
            awds_header.addr[5]=0xff;
            awds_header.type=type;
            awds_header.size=htons(p.data_length+sizeof( famouso::mw::Subject));
            buffers.push_back(boost::asio::buffer( &awds_header, sizeof(AWDS_Packet::Header)));
            buffers.push_back(boost::asio::buffer( &p.snn, sizeof( famouso::mw::Subject)));
            buffers.push_back(boost::asio::buffer( p.data, p.data_length ));

            m_socket.send( buffers);
        }

		/**
		 * @brief publish
		 *
		 * Sends the given packet.
		 *
		 * @param p packet
		 */
		void deliver_fragment( const Packet_t& p){ deliver( p, AWDS_Packet::constants::packet_type::publish_fragment); };

		/**
		 * @brief processes incoming packets
		 *
		 * @param p fetched packet is saved here
		 */
		void fetch( Packet_t& p){
            p.s =p.snn= *reinterpret_cast<SNN*>(awds_packet.data);
            p.data =  &awds_packet.data[8];
            p.data_length = ntohs(awds_packet.header.size)-sizeof(famouso::mw::Subject);
        }

		/**
		 * @brief get last SSN
		 *
		 * Returns the short network name for the last packet.
		 */
		SNN lastPacketSNN(){
		    return *reinterpret_cast<SNN*>(awds_packet.data);
        }


		/**
		 * @brief handle called on receive
		 *
		 * Will be called, whenever a packet was received.
		 *
         * \todo noch pruefen, ob auch wirklich das ganze paket da ist,
         *       ansonsten noch mal asynchron warten, bis alles da ist.
		 */
		void interrupt( const boost::system::error_code& error, size_t bytes_recvd ){
        if (!error)
		{
            if (awds_packet.header.type == AWDS_Packet::constants::packet_type::publish) {
                famouso::mw::el::IncommingEventFromNL(this);
            } else {
                std::cout<<"AWDS_Packet not supported yet"<<std::endl;
            }
		}else{
			std::cerr << "while receiving : " << error.message() << std::endl;
		}


		m_socket.async_receive( boost::asio::buffer(&awds_packet, sizeof(AWDS_Packet)),
			boost::bind( &AWDSNL::interrupt, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);

            }

	private:
		boost::asio::ip::tcp::socket m_socket;
		AWDS_Packet awds_packet;
};

}}} // namespaces

#endif /* __AWDSNL_hpp__ */
