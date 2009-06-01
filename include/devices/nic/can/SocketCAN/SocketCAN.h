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

#ifndef __SocketCAN_h__
#define __SocketCAN_h__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "object/SynchronizedBoundedBuffer.h"
#include "case/Delegate.h"
#include "util/CommandLineParameterGenerator.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>    // O_RDWR

#include "mw/nl/can/canID.h"

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
                "The device that is used"
                "(e.g. can0 (default))",
                std::string, device, "can0")

            /*!\file SocketCAN.h
             * \class SocketCAN
             *
             * \brief The generic driver interface to the SocketCAN hardware.
             *
             * This generic driver for the SocketCAN hardware allows synchronous
             * and asynchronous use. The driver has the possibility to store
             * messages to a configurable level.
             */
            class SocketCAN {
                public:

                    class MOB : private can_frame {
                            friend class SocketCAN;
                        public:
                            typedef famouso::mw::nl::CAN::detail::ID<
                                        famouso::mw::nl::CAN::detail::famouso_CAN_ID_LE_PC
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

                    };

                    explicit SocketCAN() : _can_socket(0), sbb(1000), ints_allowed(false) {
                    }

                    ~SocketCAN() {
                        close(_can_socket);
                    }

                    /*! \brief try to receive a can-message
                     *
                     * \param[out] mob is filled with last arrived message if there are one
                     *
                     * \return \li \e true if a message was received
                     *         \li \e false else
                     */
                    bool receive(MOB* mob) {
                        if (sbb.is_not_empty()) {
                            sbb.pop_back(mob);
                            return true;
                        } else {
                            return false;
                        }
                    }

                    /*! \brief blocks until a can-message on the socket is arrived */
                    void receive_blocking(MOB *mob) {
                        sbb.pop_back(mob);
                    }

                    /*! \brief send the can-message on the socket */
                    void send(MOB &mob) {
                        /* send frame */
                        if ( write(_can_socket, &mob, sizeof(mob)) != sizeof(mob)) {
                            std::cerr << "error on writing on SocketCAN socket " << std::endl;
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
                            std::cerr << "can't create SocketCAN socket" << std::endl;
                            exit(errno);
                        }
                        strcpy(ifr.ifr_name, param.device.c_str());
                        if (ioctl(_can_socket, SIOCGIFINDEX, &ifr) < 0) {
                            std::cerr << "SIOCGIFINDEX " << param.device << "no such CAN device found." << std::endl;
                            exit(errno);
                        }
                        addr.can_ifindex = ifr.ifr_ifindex;

                        if (bind(_can_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                            std::cerr << "can't bind CAN device " << param.device << std::endl;
                            exit(errno);
                        }

                        // bind the needed thread for receiving messages
                        // this allows asynchonity and callbacking for simulating
                        // receive interrupts
                        //
                        // normally that needs to be removed if the program ends, but
                        // here we rely on the underlying operating system that clears
                        // the memory of the whole program and therewith ends the thread
                        // as well
                        can_reader = new boost::thread(
                            boost::bind(&SocketCAN::CAN_Message_Reader_Thread,
                                        this));
                    }

                private:

                    /*! \brief the thread for receiving messages this allows asynchonity
                        *         and callbacking for simulating receive interrupts
                        */
                    void CAN_Message_Reader_Thread() {
                        MOB mob;
                        while (1) {
                            /* Read a message back from the CAN bus */
                            if (read(_can_socket, &mob, sizeof(MOB)) == sizeof(MOB)) {
                                // usual CAN message
                                sbb.push_front(mob);

                                // the rx_interrupt could be also executed in a extra thread
                                /*! \todo the rx_interrupt has to be synchronized with the asio
                                 *        in order to avoid inconsistencies in the data-structures
                                 *        of the event layer
                                 */
                                if (rx_Interrupt && ints_allowed)
                                    rx_Interrupt();
                            }
                        }
                    }

                    /*! \brief The data structure of the underlying CAN-Driver
                     *         and in this case it is a socket aka an int.
                     */
                    int _can_socket;

                    /*! \brief Used for storing messages in a thread safe manner */
                    object::SynchronizedBoundedBuffer<MOB>  sbb;

                    /*! \brief Thread that does the asynchronous reading of CAN messages */
                    boost::thread *can_reader;

                    /*! \brief member variable the indicates if interrupts are allowed or
                     *         not.
                     */
                    bool ints_allowed;

                public:

                    /*! \brief set the delegate that is called if an interrupt occurs */
                    void set_rx_Interrupt(famouso::util::Delegate<> f) {
                        rx_Interrupt = f;
                    }

                    /*! \brief The tx_interrupt is called if the driver is able to
                     * send new/further messages. (functionality not implemented yet)
                     *
                     * \todo Provide functionality of using tx_interrupt
                     *
                     */
                    famouso::util::Delegate<> tx_Interrupt;

                    /*! \brief the receive interrupt delegate allow binding of
                     *         ordinary C-functions as well as C++ class methods
                     */
                    famouso::util::Delegate<> rx_Interrupt;


                    /*! \brief switch interrupts for the device on
                     */
                    void interrupts_on() {
                        ints_allowed = true;
                    }

                    /*! \brief switch interrupts off for the device
                     */
                    void interrupts_off() {
                        ints_allowed = false;
                    }

            };
        } /* namespace CAN */
    } /* namespace nic */
} /* namespace device */

#endif

