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

#ifndef __PeakCAN_h__
#define __PeakCAN_h__

#include "object/SynchronizedBoundedBuffer.h"
#include "mw/nl/can/GenericReceiveInterrupt.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>    // O_RDWR
#include <libpcan.h>

#include "mw/nl/can/canID.h"

namespace device {
    namespace nic {
        namespace CAN {

            /*! \file PeakCAN.h
             *
             * \class PeakCAN
             *
             * \brief The generic driver interface to the PeakCAN hardware.
             *
             * This generic driver for the PeakCAN hardware allows synchronous
             * and asynchronous use. The driver has the possibility to store
             * messages to a configurable level.
             *
             * \param[in] device defines the %device on a linux system where
             * the CAN hardware is pluged
             * \param[in] wBTR0BTR1 -- the baudrate that is set (see libpcan.h)
             * \param[in] elements describe the count messages that are stored at
             * maximum
             *
             * \todo develop a trait that allows using that interface on linux
             * as well as windows. Especially the CAN_OPEN function differs
             * between the platforms.
             *
             */

            template < const char *&device, uint16_t wBTR0BTR1, uint32_t elements = 1000 >
            class PeakCAN {
                public:

                    /*! \todo die CAN-Message Abstraction ist noch ungluecklich.
                     *        Hier muss noch einiges am Konzept einer Nachricht
                     *        geaendert werden, um eine allgemeine Nachricht zu
                     *        haben. Es bedarf eines einheitlichen Interfaces.
                     */
                class MOB : private TPCANMsg {
                            friend class PeakCAN<device, wBTR0BTR1, elements> ;
                        public:
                            typedef famouso::mw::nl::CAN::PEAK::ID  IDType;

                            void extended() {
                                MSGTYPE = MSGTYPE_EXTENDED;
                            }
                            IDType& id() {
                                return *reinterpret_cast<IDType*>(&ID);
                            }
                            uint8_t& len() {
                                return LEN;
                            }
                            void len(uint8_t l) {
                                LEN = l;
                            }
                            uint8_t *data() {
                                return DATA;
                            }
                            void data(uint8_t *) {}
                            uint8_t &data(uint8_t i) {
                                return &DATA[i];
                            }

                    };

                    PeakCAN() : sbb(elements) {
                        ints_allowed = false;
                    }
                    ~PeakCAN() {
                        CAN_Close(handle);
                    }

                    bool receive(MOB* mob) {
                        if (sbb.is_not_empty()) {
                            sbb.pop_back(mob);
                            return true;
                        } else {
                            return false;
                        }
                    }

                    void receive_blocking(MOB *mob) {
                        sbb.pop_back(mob);
                    }

                    void send(MOB &mob) {
                        if ((errno = CAN_Write(handle, &mob))) {
                            std::cerr << "CAN_Read() error" << std::endl;
                            std::cerr << "closing application" << std::endl;
                            exit(0);
                        }
                    }

                    void init() {
                        errno = 0;
                        /* open CAN port */
                        // this needs to be variable to allow using the interface
                        //  on linux and windows in the same way
                        handle = LINUX_CAN_Open(device, O_RDWR);
                        if (!handle) {
                            std::cerr << "can't open CAN device " << device << std::endl;
                            exit(errno);
                        }

                        errno = CAN_Init(handle, wBTR0BTR1, CAN_INIT_TYPE_EX);
                        if (errno) {
                            std::cerr << "CAN_Init() fails" << std::endl;
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
                            boost::bind(&PeakCAN< device, wBTR0BTR1, elements>::CAN_Message_Reader_Thread,
                                        this));
                    }

                private:

                    /*! \brief the thread for receiving messages this allows asynchonity
                        *         and callbacking for simulating receive interrupts
                        */
                    void CAN_Message_Reader_Thread() {
                        MOB mob;
                        while (1) {
                            uint32_t status;
                            if ((errno = CAN_Read(handle, &mob))) {
                                std::cerr << "CAN_Read() error" << std::endl;
                                std::cerr << "closing application" << std::endl;
                                exit(0);
                            } else {
                                // check if a CAN status is pending
                                if (mob.MSGTYPE & MSGTYPE_STATUS) {
                                    status = CAN_Status(handle);
                                    if ((int)status < 0) {
                                        errno = nGetLastError();
                                        std::cerr << "CAN error=" << errno << std::endl;
                                        std::cerr << "closing application" << std::endl;
                                        exit(0);
                                    } else
                                        std::cerr << "pending CAN status 0x." << status << " read." << std::endl;
                                } else {
                                    // usual CAN message
                                    //std::cout<<"New Message arrived and Quened"<<std::endl;
                                    sbb.push_front(mob);

                                    // the rx_interrupt could be also executed in a extra thread
                                    /*! \todo the rx_interrupt has to be synchronized with the asio
                                     *        in order to avoid inconsistencies in the data-structures
                                     *        of the event layer
                                     */
                                    if (device::nic::CAN::detail::rx_Interrupt && ints_allowed)
                                        device::nic::CAN::detail::rx_Interrupt();
                                }
                            }
                        }
                    }

                    /*! \brief The data structure of the underlying CAN-Driver */
                    HANDLE handle;

                    /*! \brief Used for storing messages in a thread safe manner */
                    object::SynchronizedBoundedBuffer<MOB>  sbb;

                    /*! \brief Thread that does the asynchronous reading of CAN messages */
                    boost::thread *can_reader;

                public:

                    void set_rx_Interrupt(famouso::util::Delegate<> f) {
                        device::nic::CAN::detail::rx_Interrupt = f;
                    }

                    /*! \brief The tx_interrupt is called if the driver is able to
                     * send new/further messages. (functionality not implemented yet)
                     *
                     * \todo Provide functionality of using tx_interrupt
                     *
                     */
                    famouso::util::Delegate<> tx_Interrupt;


                    bool ints_allowed;
                    void interrupts_on() {
                        ints_allowed = true;
                    }

                    void interrupts_off() {
                        ints_allowed = false;
                    }

            };
        } /* namespace CAN */
    } /* namespace nic */
} /* namespace device */

#endif

