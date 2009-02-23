#ifndef __UDPMultiCastNL_hpp__
#define __UDPMultiCastNL_hpp__

#include <stdio.h>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <string>
#include "mw/nl/BaseNL.h"
#include "mw/nl/Packet.h"
#include "util/ios.h"
#include "mw/common/Subject.h"

namespace famouso {
    namespace mw {
        namespace nl {

            class UDPMultiCastNL : public BaseNL, boost::noncopyable {
                public:

                    struct info {
                        enum {
                            payload = 8192 - sizeof( famouso::mw::Subject ),
                        };
                    };

                    // type of an address
                    typedef boost::asio::ip::address SNN;

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
                            : m_socket( famouso::util::ios::instance() ), port( p ),
                            m_endpoint_listen( boost::asio::ip::address::from_string("0.0.0.0"), port ) {
                        famouso::util::impl::start_ios();
                    }

                    /**
                     * @brief destructor
                     *
                     *  Closes the socket.
                     */
                    ~UDPMultiCastNL() {
                        m_socket.close( );
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
                    void bind( const famouso::mw::Subject &s, SNN &snn );

                    /**
                     * @brief called by deliver
                     *
                     * Is called when sending is finished.
                     * @param error tells if sending was successful
                     * @param bytes_recvd the amount of bytes that were sent
                     */
                    void handle_send_to( const boost::system::error_code& error, size_t bytes_recvd );

                    /**
                     * @brief Sends the given packet.
                     *
                     * @param p packet
                     */
                    void deliver( const Packet_t& p );

                    /**
                     * @brief Sends the given packet.
                     *
                     * @param p packet
                     */
                    void deliver_fragment( const Packet_t& p) {
                        deliver( p );
                    };

                    /**
                     * @brief processes incoming packets
                     *
                     * @param p fetched packet is saved here
                     */
                    void fetch( Packet_t& p);

                    /**
                     * @brief get last SSN
                     *
                     * Returns the short network name for the last packet.
                     */
                    SNN lastPacketSNN();


                    /**
                     * @brief handle called on receive
                     *
                     * Will be called, whenever a packet was received.
                     *
                     */
                    void interrupt( const boost::system::error_code& error, size_t bytes_recvd );

                private:
                    boost::asio::ip::udp::socket m_socket;
                    const int port;
                    boost::asio::ip::udp::endpoint m_endpoint_listen;
                    boost::asio::ip::udp::endpoint m_endpoint_from;
                    uint8_t m_buffer[sizeof( famouso::mw::Subject )+info::payload]; // Subject and Payload have to be sent
                    Packet_t m_incoming_packet;
            };

        }
    }
} // namespaces


#endif /* __UDPMultiCastNL_hpp__ */
