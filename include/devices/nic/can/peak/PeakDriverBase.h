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

#ifndef __PeakDriverBase_h__
#define __PeakDriverBase_h__

#if defined(LINUX)
#include <errno.h>
#include <fcntl.h>    // O_RDWR
#include <libpcan.h>
#elif defined(WIN32)
#if defined(PEAKUSB)
#include "Pcan_usb.h"
#elif defined(PEAKPCI)
#include "Pcan_pci.h"
#else
#error "You have to define either PEAKUSB or PEAKPCI to choose the used Peak driver"
#endif
#else
#error "PEAK driver within FAMOUSO not support on that platform"
#endif

#include "mw/nl/can/canID.h"

namespace device {
    namespace nic {
        namespace CAN {

            /*! \brief PeakDriverBase defines the %CAN message %object structure and
             *         the access methods.
             */
            struct PeakDriverBase {

                /*! \brief MOB is derived from the concrete message %object and it
                 *         defines access methods.
                 */
                class MOB : public TPCANMsg {
                    public:
                        /*! \brief the typedef defines the ID in a style that is
                         *         easy to use for FAMOUSO. Especially, access to
                         *         the different parts within the ID is supported.
                         */
                        typedef famouso::mw::nl::CAN::detail::ID <
                                    famouso::mw::nl::CAN::detail::famouso_CAN_ID_LE_PC
                                > IDType;

                        /*! set the extended message format bit                         */
                        void extended() {
                            MSGTYPE = MSGTYPE_EXTENDED;
                        }

                        /*! convert the ID to the correct Type and return it            */
                        IDType& id() {
                            return *reinterpret_cast<IDType*>(&ID);
                        }

                        /*! get the length of the payload of the %CAN message %object     */
                        uint8_t& len() {
                            return LEN;
                        }

                        /*! set the length of the payload of the %CAN message %object     */
                        void len(uint8_t l) {
                            LEN = l;
                        }

                        /*! enables access to the payload of the %CAN message %object     */
                        uint8_t *data() {
                            return DATA;
                        }

                        /*! the constructor creates a %CAN message %object with ID zero   */
                        explicit MOB() {
                            ID = 0;
                        }
                };
            };

            /*! \brief defines an array containing different baudrates supported by the
             *         PeakCAN devices. It is used, to allow setting of the baudrate of
             *         the %device via commandline parameters
             */
            const uint16_t baudrates[] = { CAN_BAUD_1M,
                                           CAN_BAUD_500K,
                                           CAN_BAUD_250K,
                                           CAN_BAUD_125K,
                                           CAN_BAUD_100K,
                                           CAN_BAUD_50K,
                                           CAN_BAUD_20K,
                                           CAN_BAUD_10K,
                                           CAN_BAUD_5K
                                         };

        } /* namespace CAN */
    } /* namespace nic */
} /* namespace device */

#endif

