/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __SocketCANDriver_h__
#define __SocketCANDriver_h__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "util/CommandLineParameterGenerator.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "debug.h"
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>    // O_RDWR

#include "mw/nl/can/canID_LE_PC.h"

#include <linux/can.h>
#include <linux/can/raw.h>
/* At time of writing, these constants are not defined in the headers */
#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif


namespace device {
    namespace nic {
        namespace CAN {

             CLP1(SocketCANOptions,
                "Socket CAN Driver",
                "scan,s",
                "The device that is used\n"
                "(e.g. can0 (default))",
                std::string, device, "can0");

            /*!
             * \brief The generic driver interface to the SocketCAN hardware.
             */
            template< typename IDDesc=famouso::mw::nl::CAN::detail::famouso_CAN_ID_LE_PC>
            class SocketCANDriver {
                public:

                    class MOB : private can_frame {
                        public:
                            typedef famouso::mw::nl::CAN::detail::ID<
                                        IDDesc
                                    > IDType;

                            void extended() {
                                can_id |= CAN_EFF_FLAG;
                            }
                            IDType& id() {
                                return *reinterpret_cast<IDType*>(&can_id);
                            }
                            uint8_t& len() {
                                return can_dlc;
                            }
                            void len(uint8_t l) {
                                can_dlc = l;
                            }
                            uint8_t *data() {
                                return can_frame::data;
                            }
                            void data(uint8_t *) {}
                            uint8_t &data(uint8_t i) {
                                return can_frame::data[i];
                            }
                            explicit MOB() {can_id=0;}
                    };

                    explicit SocketCANDriver() : _can_socket(0) {
                    }

                    ~SocketCANDriver() {
                        close(_can_socket);
                    }

                    /*! try receiving a %CAN message %object
                     *
                     * \param[out] mob is filled with a received message
                     *
                     * \return true if, a message was received
                     * \return false otherwise
                     */
                    bool read(MOB& mob) {
                        if (::read(_can_socket, &mob, sizeof(MOB)) == sizeof(MOB)) {
                            return true;
                        } else {
                            return false;
                        }
                    }

                    /*! \brief transmits a %CAN message %object
                     *
                     * \param[in] mob that has to be transmitted
                     */
                    void write(MOB &mob) {
                        /* send frame */
                        if ( ::write(_can_socket, &mob, sizeof(mob)) != sizeof(mob)) {
                            ::logging::log::emit< ::logging::Error>()
                                << "error on writing on SocketCAN socket "
                                << ::logging::log::endl;
                            exit(errno);
                        }
                    }

                    /*! \brief initalize the driver by trying to open and bind a socket */
                    void init() {
                        CLP::config::SocketCANOptions::Parameter param;
                        CLP::config::SocketCANOptions::instance().getParameter(param);
                        /* open CAN port */
                        struct sockaddr_can addr;
                        struct ifreq ifr;
                        if ((_can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
                            ::logging::log::emit< ::logging::Error>()
                                << "can't create SocketCAN socket"
                                << ::logging::log::endl;
                            exit(errno);
                        }
                        strcpy(ifr.ifr_name, param.device.c_str());
                        if (ioctl(_can_socket, SIOCGIFINDEX, &ifr) < 0) {
                            ::logging::log::emit< ::logging::Error>()
                                << "SIOCGIFINDEX " << param.device.c_str()
                                << "no such CAN device found."
                                << ::logging::log::endl;
                            exit(errno);
                        }
                        addr.can_ifindex = ifr.ifr_ifindex;

                        if (bind(_can_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                            ::logging::log::emit< ::logging::Error>()
                                << "can't bind CAN device " << param.device.c_str()
                                << ::logging::log::endl;
                            exit(errno);
                        }
                   }

                private:
                    /*! \brief The data structure of the underlying CAN-Driver
                     *         and in this case it is a socket aka an int.
                     */
                    int _can_socket;
           };

        } /* namespace CAN */
    } /* namespace nic */
} /* namespace device */

#endif

