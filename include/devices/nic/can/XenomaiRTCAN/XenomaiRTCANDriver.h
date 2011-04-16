/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __XenomaiRTCANDriver_h__
#define __XenomaiRTCANDriver_h__

#include <stdio.h>
#include <rtdm/rtcan.h>
#include <string.h>
#include <sys/mman.h>

#include "util/CommandLineParameterGenerator.h"

#include "debug.h"

#include "mw/nl/can/canID_LE_PC.h"

namespace device {
    namespace nic {
        namespace CAN {

            CLP1(XenomaiRTCANOptions,
                 "XenomaiRT CAN Driver",
                 "scan,s",
                 "The device that is used\n"
                 "(e.g. rtcan0 (default))",
                 std::string, device, "rtcan0");

            /*!
             * \brief The generic driver interface to the XenomaiRTCAN hardware.
             */
            template < typename IDDesc = famouso::mw::nl::CAN::detail::famouso_CAN_ID_LE_PC >
            class XenomaiRTCANDriver {
                public:

                    class MOB : private can_frame {
                        public:
                            typedef famouso::mw::nl::CAN::detail::ID <
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
                            explicit MOB() {
                                can_id = 0;
                            }
                    };

                    explicit XenomaiRTCANDriver() : _can_socket(0) {
                    }

                    ~XenomaiRTCANDriver() {
                        rt_dev_close(_can_socket);
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
                        if ((ret = rt_dev_recv(_can_socket, &mob, sizeof(MOB), 0))  != sizeof(MOB) ) {
                            report_error_and_exit(ret,"__FILE__:__LINE__ : rt_dev_recv ");
                        }
                        return true;
                    }

                    /*! \brief transmits a %CAN message %object
                     *
                     * \param[in] mob that has to be transmitted
                     */
                    void write(MOB &mob) {
                        /* send frame */
                        int ret = 0;
                        if ( (ret = rt_dev_send(_can_socket, &mob, sizeof(mob), 0)) != sizeof(MOB)) {
                            report_error_and_exit(ret,"__FILE__:__LINE__ : rt_dev_send ");
                        }
                    }

                    /*! \brief initalize the driver by trying to open and bind a socket */
                    void init() {
                        CLP::config::XenomaiRTCANOptions::Parameter param;
                        CLP::config::XenomaiRTCANOptions::instance().getParameter(param);
                        /* open CAN port */
                        struct sockaddr_can addr;
                        struct ifreq ifr;
                        if ((_can_socket = rt_dev_socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
                            ::logging::log::emit< ::logging::Error>()
                            << "can't create XenomaiRTCAN socket"
                            << ::logging::log::endl;
                            exit(errno);
                        }
                        strcpy(ifr.ifr_name, param.device.c_str());
                        if (rt_dev_ioctl(_can_socket, SIOCGIFINDEX, &ifr) < 0) {
                            ::logging::log::emit< ::logging::Error>()
                            << "SIOCGIFINDEX " << param.device.c_str()
                            << "no such CAN device found."
                            << ::logging::log::endl;
                            exit(errno);
                        }
                        addr.can_family = AF_CAN;
                        addr.can_ifindex = ifr.ifr_ifindex;

                        if (rt_dev_bind(_can_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                            ::logging::log::emit< ::logging::Error>()
                            << "can't bind CAN device " << param.device.c_str()
                            << ::logging::log::endl;
                            exit(errno);
                        }
                    }

                protected:
                    /*! \brief The data structure of the underlying CAN-Driver
                     *         and in this case it is a socket aka an int.
                     */
                    int _can_socket;

            };

            void fnc() __attribute__((constructor));
            void fnc() {
                mlockall (MCL_CURRENT | MCL_FUTURE);
            }
        } /* namespace CAN */
    } /* namespace nic */
} /* namespace device */

#endif

