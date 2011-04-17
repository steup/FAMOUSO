/*******************************************************************************
 *
 * Copyright (c) 2011 Philipp Werner <philipp.werner@st.ovgu.de>
 *                    Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __TIMESTAMPINGXENOMAIRTCAN_H_E44BF0469640DF__
#define __TIMESTAMPINGXENOMAIRTCAN_H_E44BF0469640DF__

#include "devices/nic/can/PAXCAN/SimulateInterruptViaThreadAdapter.h"
#include "devices/nic/can/PAXCAN/PAXCAN.h"
#include "devices/nic/can/XenomaiRTCAN/XenomaiRTCANDriver.h"
#include <rtdm/rtdm.h>

namespace device {
    namespace nic {
        namespace CAN {

            /*!
             *  \brief  XenomaiRTCAN driver taking timestamp on reveive
             */
            class RecvTimestampingXenomaiRTCANDriver : public XenomaiRTCANDriver<> {

                protected:
                    struct msghdr msg;
                    struct iovec iov;

                public:

                    /// Return last timestamp (WARNING: may be overwritten by another thread before read!!!!)
                    static nanosecs_abs_t & last_timestamp() {
                        static nanosecs_abs_t ts = 0;
                        return ts;
                    }

                    /// Initialization
                    void init() {
                        XenomaiRTCANDriver<>::init();
                        memset(&msg, 0, sizeof(msg));

                        msg.msg_iov = &iov;
                        msg.msg_iovlen = 1;
                        msg.msg_control = (void *)&last_timestamp();
                        msg.msg_controllen = sizeof(nanosecs_abs_t);


                        int ret = rt_dev_ioctl(_can_socket, RTCAN_RTIOC_TAKE_TIMESTAMP, RTCAN_TAKE_TIMESTAMPS);
                        if (ret) {
                            report_error_and_exit(ret, "__FILE__:__LINE__: rt_dev_ioctl ");
                        }
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
                        iov.iov_base = (void *)&mob;
                        iov.iov_len = sizeof(can_frame_t);
                        ret = rt_dev_recvmsg(_can_socket, &msg, 0);
                        //printf("XenoDriver: receive\n");
                        if (ret != sizeof(MOB) ) {
                            report_error_and_exit(ret, "__FILE__:__LINE__: rt_dev_recvmsg ");
                        }
                        return true;
                    }
            };

            typedef SimulateInterruptViaThreadAdapter< PAXCAN < RecvTimestampingXenomaiRTCANDriver > > RecvTimestampingXenomaiRTCAN;


            /*!
             *  \brief  XenomaiRTCAN driver taking timestamp on send
             */
            class SendTimestampingXenomaiRTCANDriver : public RecvTimestampingXenomaiRTCANDriver  {
                    typedef RecvTimestampingXenomaiRTCANDriver Base;
                    class LoopbackSock : public RecvTimestampingXenomaiRTCANDriver {
                        public:
                            void init(nanosecs_abs_t & timestamp_storage) {
                                RecvTimestampingXenomaiRTCANDriver::init();
                                msg.msg_control = (void *)&timestamp_storage;
                            }
                    };

                    LoopbackSock loopback;

                public:
                    /// Return last timestamp
                    static nanosecs_abs_t & last_timestamp() {
                        static nanosecs_abs_t ts = 0;
                        return ts;
                    }

                    /// Initialization
                    void init() {
                        Base::init();
                        loopback.init(last_timestamp());
                    }

                    void log_mob(MOB & loopback_mob) {
                            printf("mob id %x, len %d, data", loopback_mob.id()._value, loopback_mob.len());
                            for (int i = 0; i < loopback_mob.len(); i++)
                                printf(" %x", loopback_mob.data()[i]);
                            printf("\n");
                    }

                    void write(MOB& mob) {
                        Base::write(mob);
                        //log_mob(mob);
                        MOB loopback_mob;
                        ::logging::log::emit() << "loopback";
                        do {
                            loopback.read(loopback_mob);
                            ::logging::log::emit() << '.';
                            //log_mob(loopback_mob);
                        } while ((mob.id()._value & 0x1fffffff) != (loopback_mob.id()._value & 0x1fffffff) ||
                                 mob.len() != loopback_mob.len() ||
                                 memcmp(mob.data(), loopback_mob.data(), mob.len()));
                        ::logging::log::emit() << '\n';
                    }
            };

            typedef SimulateInterruptViaThreadAdapter< PAXCAN < SendTimestampingXenomaiRTCANDriver > > SendTimestampingXenomaiRTCAN;

        } /* namespace CAN */
    } /* namespace nic */
} /* namespace device */


#endif // __TIMESTAMPINGXENOMAIRTCAN_H_E44BF0469640DF__

