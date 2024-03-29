/*******************************************************************************
 *
 * Copyright (c) 2008-2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *               2009-2010 Michael Kriese <kriese@cs.uni-magdeburg.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/

#ifndef __AWDSNL_hpp__
#define __AWDSNL_hpp__

#include "mw/nl/DistinctNL.h"
#include "mw/nl/Packet.h"
#include "mw/afp/Config.h"
#include "mw/common/Subject.h"
#include "mw/nl/awds/AWDS_Packet.h"
#include "mw/nl/awds/NodeRepository.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <map>

namespace famouso {
    namespace mw {
        namespace nl {

            /** \namespace famouso::mw::nl::awds
             *  \brief The namespace awds holds classes, structures and functions for the AWDS network.
             */

            using namespace awds;

            /*! \brief The AWDS network layer provides functionality for sending
             *         packets over the AWDS network.
             */
            class AWDSNL: public DistinctNL, boost::noncopyable {
                public:

                    /*! \brief A Struct for holding informational constants.
                     */
                    struct info {
                            enum {
                                /*! \brief The maximum payload an AWDS packet can have
                                 *         (without the subject).
                                 */
                                payload = AWDS_Packet::constants::packet_size::payload - sizeof(famouso::mw::Subject)
                            };
                    };

                    /** \brief type of an address */
                    typedef famouso::mw::Subject SNN;

                    /** \brief type of a packet */
                    typedef Packet<SNN> Packet_t;

                    /** \brief default AFP %configuration of this network layer (see \ref afp_config_nl) */
                    typedef afp::MultiSubjectESeqReorderDuplicateConfig<SNN> AFP_Config;

                    /**
                     * \brief default constructor
                     */
                    AWDSNL();

                    /**
                     * \brief destructor
                     *
                     *  Closes the socket.
                     *
                     */
                    ~AWDSNL() throw ();

                    /**
                     * Sets the options for the socket and starts receiving.
                     */
                    void init();

                    /**
                     * \brief bind a subject
                     *
                     * \param s subject
                     * \param snn bound address
                     */
                    void bind(const famouso::mw::Subject &s, SNN &snn);

                    /**
                     * \brief deliver a packet.
                     *
                     * Sends the given packet.
                     *
                     * \param[in] p packet
                     */
                    void deliver(const Packet_t& p);

                    /**
                     * \brief processes incoming packets
                     *
                     * \param p taken packet is saved here
                     */
                    void take(Packet_t& p);

                    /**
                     * \brief get last SSN
                     *
                     * Returns the short network name for the last packet.
                     */
                    SNN lastPacketSNN();

                private:
                    /**
                     * \brief handle called on receive
                     *
                     * Will be called, whenever a packet was received.
                     */
                    void interrupt(const boost::system::error_code& error, size_t bytes_recvd);

                    /**
                     * \brief Notifies other nodes about subscribers on this node.
                     *
                     * \param error a reference to an error.
                     */
                    void announce_subscriptions(const boost::system::error_code& error);

                    boost::asio::ip::tcp::socket m_socket; /**< The socket for the IPC connettion with AWDS. */
                    boost::asio::deadline_timer timer_; /**< The timer for subscribtion renewval. */
                    AWDS_Packet awds_packet; /**< The actual packet to work on */
                    std::map<SNN, ComparableAttributeSet<>::type> subscriptions; /**< A list of subjects from all local subscribers. */
                    NodeRepository::type &_repo; /**< The node repository for registering and checking nodes before publishing. */
                    bool next_packet_is_full_packet; /**< With the next data from AWDS the packet is full. */
                    int interval; /**< Refresh time in secods to renew subscribtions to other nodes. */
                    bool flowMgmtAvail; /**< Flow Manager available (default false). */
                    int _maxAge; /**< The timespan in seconds when a node is marked as offline. */
                    int8_t _flowPrio; /**< The AWDS Flow Priority. */
            };

        } // namespace nl
    } // namespace mw
} // namespace famouso

#endif /* __AWDSNL_hpp__ */
