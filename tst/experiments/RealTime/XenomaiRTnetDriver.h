/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2011 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __XenomaiRTnetDriver_h__
#define __XenomaiRTnetDriver_h__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "util/CommandLineParameterGenerator.h"

#include "debug.h"

#include "devices/nic/can/PAXCAN/SimulateInterruptViaThreadAdapter.h"
#include "PAXRTnet.h"

namespace device {
    namespace nic {
        namespace ethernet {

            CLP1(XenomaiRTnetOptions,
                 "XenomaiRT CAN Driver",
                 "interface,i",
                 "The network interface that is used\n"
                 "(e.g. rteth0 (default))",
                 std::string, device, "rteth0");

            /*!
             * \brief Driver for ethernet communication via Xenomai/RTnet
             * \see EthernetNL
             */
            class XenomaiRTnetDriver {
                public:

                    /// Message object type
                    class MOB {
                        public:
                            enum { max_io_len = 1514 };

                            explicit MOB() {
                                struct ether_header * hdr = reinterpret_cast<struct ether_header *>(buffer);
                                memset(hdr->ether_dhost, 0xFF, ETH_HLEN);
                                hdr->ether_type = htons(0x1234);
                                payload_len = 0;
                            }

                            /// Payload length
                            uint16_t& len() {
                                return payload_len;
                            }

                            /// Total length including header
                            uint16_t io_len() {
                                return payload_len + 14;
                            }

                            /// Payload data
                            uint8_t *data() {
                                return buffer + sizeof(struct ether_header);
                            }
                        private:
                            uint8_t buffer[max_io_len];
                            uint16_t payload_len;
                    };

                    /// Constructor
                    explicit XenomaiRTnetDriver() : sock(0) {
                    }

                    /// Destructor
                    ~XenomaiRTnetDriver() {
                        close(sock);
                    }

                    void report_error_and_exit(int error_value, const char* reporter) {
                        const char *error = 0;
                        switch (error_value) {
                            case -ETIMEDOUT:
                                error = "timed out";
                                break;
                            case -EBADF:
                                error = "aborted because socket was closed";
                                break;
                            default:
                                error = strerror (-error_value);
                                break;
                        }

                        ::logging::log::emit< ::logging::Error>() << reporter
                            << error << ::logging::log::endl;
                        exit(errno);
                    }

                    /*! try receiving a %CAN message %object
                     *
                     * \param[out] mob is filled with a received message
                     *
                     * \return true if, a message was received
                     * \return false otherwise
                     */
                    bool read(MOB& mob) {
                        int ret = 0;
                        if ((ret = recv(sock, &mob, MOB::max_io_len, 0)) < 0) {
                            report_error_and_exit(ret,"__FILE__:__LINE__ : recv ");
                        }
                        mob.len() = ret;
                        return true;
                    }

                    /*! \brief transmits a %CAN message %object
                     *
                     * \param[in] mob that has to be transmitted
                     */
                    void write(MOB &mob) {
                        int ret = 0;
                        if ( (ret = send(sock, &mob, mob.io_len(), 0)) < 0) {
                            report_error_and_exit(ret,"__FILE__:__LINE__ : send ");
                        }
                        FAMOUSO_ASSERT(ret == mob.io_len());
                    }

                    /*! \brief initalize the driver by trying to open and bind a socket */
                    void init() {
                        if ((sock = socket(PF_PACKET, SOCK_RAW, htons(0x1234))) < 0) {
                            ::logging::log::emit< ::logging::Error>()
                                << "can't create XenomaiRTnet socket"
                                << ::logging::log::endl;
                            perror("socket cannot be created");
                            exit(errno);
                        }

                        struct ifreq ifr;
                        memset(&ifr, 0, sizeof(ifr));
                        strncpy(ifr.ifr_name, "rteth0", IFNAMSIZ);
                        if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
                            perror("cannot get interface index");
                            close(sock);
                            exit(1);
                        }

                        struct sockaddr_ll addr;
                        memset(&addr, 0, sizeof(addr));
                        addr.sll_family   = AF_PACKET;
                        addr.sll_protocol = htons(0x1234);
                        addr.sll_ifindex  = ifr.ifr_ifindex;

                        if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                            perror("cannot bind to local ip/port");
                            close(sock);
                            exit(1);
                        }
                    }

                protected:
                    /*! \brief The data structure of the underlying CAN-Driver
                     *         and in this case it is a socket aka an int.
                     */
                    int sock;

            };

            void fnc() __attribute__((constructor));
            void fnc() {
                mlockall (MCL_CURRENT | MCL_FUTURE);
            }

            typedef CAN::SimulateInterruptViaThreadAdapter <
                        PAXRTnet < XenomaiRTnetDriver > >  XenomaiRTnet;

        } /* namespace ethernet */
    } /* namespace nic */
} /* namespace device */

#endif

