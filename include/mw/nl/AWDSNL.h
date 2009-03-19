#ifndef __AWDSNL_hpp__
#define __AWDSNL_hpp__

#include <stdio.h>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>
#include <vector>
#include <iterator>

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

template<uint16_t Port=8555, uint16_t interval=60>
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
		AWDSNL() : m_socket( famouso::util::ios::instance() ),
                   timer_( famouso::util::ios::instance(),
                           boost::posix_time::seconds(interval)) {}

		/**
		 * @brief destructor
		 *
		 *  Closes the socket.
		 *
		 */
		~AWDSNL() throw() {
			m_socket.close( );
		}

		/**
		 * Sets the options for the socket and starts receiving.
		 */
		void init() {
            famouso::util::impl::start_ios();
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), Port);
            boost::system::error_code ec;
            m_socket.connect(endpoint, ec);
            if (ec) {
               // An error occurred.
                throw "could not connect to AWDS-Network";
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
            // try to detect subscription channel because in AWDS its get special treatment
            // this test works only due to the fact that the SNN is equal to the famouso::mw::Subjcet
            if (p.snn == famouso::mw::Subject("SUBSCRIBE")) {
                subscriptions.push_front(famouso::mw::Subject(p.data));
                subscriptions.sort();
                subscriptions.unique();

                timer_.cancel();
                announce_subscriptions(boost::system::error_code());
            } else {
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
            p.snn= famouso::mw::Subject(awds_packet.data);
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
            switch (awds_packet.header.type) {
                case AWDS_Packet::constants::packet_type::publish :
                case AWDS_Packet::constants::packet_type::publish_fragment : {
                        famouso::mw::el::IncommingEventFromNL(this);
                        break;
                }
                case AWDS_Packet::constants::packet_type::subscribe : {
                    /*! \todo implement subscription announcement see protocol specification in doc */
                    break;
                }
                default: std::cout<<"AWDS_Packet not supported yet"<<std::endl;
            }

            m_socket.async_receive( boost::asio::buffer(&awds_packet, sizeof(AWDS_Packet)),
                                    boost::bind( &AWDSNL::interrupt, this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred));

		}else{
			std::cerr << "AWDS-Network : " << error.message() << std::endl;
            throw "AWDS-Network disconnected likely";
		}


        }

	private:
        void announce_subscriptions(const boost::system::error_code& error){
            if (!error) {// != boost::asio::error::operation_aborted)

            if (subscriptions.size()) {
            uint16_t count_subj=htons(subscriptions.size());
            std::vector<boost::asio::const_buffer> buffers;
            uint32_t reserved=htonl(20);
            AWDS_Packet::Header awds_header;
            awds_header.addr[0]=0xff;
            awds_header.addr[1]=0xff;
            awds_header.addr[2]=0xff;
            awds_header.addr[3]=0xff;
            awds_header.addr[4]=0xff;
            awds_header.addr[5]=0xff;
            awds_header.type=AWDS_Packet::constants::packet_type::subscribe;
            awds_header.size=htons(sizeof(uint32_t)+2+htons(count_subj)*sizeof(famouso::mw::Subject));
            buffers.push_back(boost::asio::buffer( &awds_header, sizeof(AWDS_Packet::Header)));
            buffers.push_back(boost::asio::buffer( &reserved, sizeof( uint32_t)));
            buffers.push_back(boost::asio::buffer( &count_subj, sizeof( uint16_t)));

            for(std::list<SNN>::const_iterator i=subscriptions.begin(); i!=subscriptions.end(); ++i) {
                buffers.push_back(boost::asio::buffer( &(*i), sizeof( famouso::mw::Subject)));
            }

            m_socket.send( buffers);
            }

            timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(interval));
            timer_.async_wait(boost::bind(&AWDSNL::announce_subscriptions, this, boost::asio::placeholders::error));
       }
        }
		boost::asio::ip::tcp::socket m_socket;
        boost::asio::deadline_timer timer_;
		AWDS_Packet awds_packet;
        std::list<SNN> subscriptions;
};

}}} // namespaces

#endif /* __AWDSNL_hpp__ */
