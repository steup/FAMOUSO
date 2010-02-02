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

#ifndef __PeakDriverLinuxTrait_h__
#define __PeakDriverLinuxTrait_h__

#include "debug.h"

#include "devices/nic/can/peak/PeakDriverBase.h"
#include "util/CommandLineParameterGenerator.h"

namespace device {
    namespace nic {
        namespace CAN {

            CLP2(PEAKOptions,
                 "Peak CAN Driver",
                 "pcan,p",
                 "The device that is used and the baudrate in the form of device:baudrate"
                 "Values:\n"
                 "  Device:   \tdevice that has to be used\n"
                 "  Baudrate: \tlegal values are\n"
                 "               0 =   1 MBit/s\n"
                 "               1 = 500 kBit/s\n"
                 "               2 = 250 kBit/s\n"
                 "               3 = 125 kBit/s\n"
                 "               4 = 100 kBit/s\n"
                 "               5 =  50 kBit/s\n"
                 "               6 =  20 kBit/s\n"
                 "               7 =  10 kBit/s\n"
                 "               8 =   5 kBit/s\n"
                 "(e.g. /dev/pcan32:2 (default))",
                 std::string, device, "/dev/pcan32",
                 int, baudrate, 2);

            /*! \brief PeakDriverLinuxTrait abstacts from the low-level Peak Linux driver
             *         API to a generic API, that is used from device::nic::can::PeakDriver.
             */
            class PeakDriverLinuxTrait : public PeakDriverBase {
                    /*! \brief The data structure of the underlying %CAN-Driver */
                    HANDLE handle;
                public:

                    /*! The destructor close the driver */
                    ~PeakDriverLinuxTrait() {
                        CAN_Close(handle);
                    }

                    /*! initialize the driver in the Linux specific way */
                    void init() {
                        CLP::config::PEAKOptions::Parameter param;
                        CLP::config::PEAKOptions::instance().getParameter(param);
                        if ( (param.baudrate < 0) || (param.baudrate > 8) ) {
                            ::logging::log::emit< ::logging::Error>()
                                << "Error: parameter baudrate out of Range"
                                << ::logging::log::endl;
                            exit(0);
                        }

                        errno = 0;
                        /* open CAN port */
                        // this needs to be variable to allow using the interface
                        //  on linux and windows in the same way
                        handle = LINUX_CAN_Open(param.device.c_str(), O_RDWR);
                        if (!handle) {
                            ::logging::log::emit< ::logging::Error>()
                                << "can't open CAN device " << param.device.c_str()
                                << ::logging::log::endl;
                            exit(errno);
                        }

                        errno = CAN_Init(handle, baudrates[param.baudrate], CAN_INIT_TYPE_EX);
                        if (errno) {
                            ::logging::log::emit< ::logging::Error>()
                                << "CAN_Init() fails"
                                << ::logging::log::endl;
                            exit(errno);
                        }
                    }

                    /*! API mapping of the %CAN read message %object */
                    DWORD read_mob(MOB &mob) {
                        return CAN_Read(handle, &mob);
                    }

                    /*! API mapping of the %CAN write message %object */
                    DWORD write_mob(MOB &mob) {
                        return CAN_Write(handle, &mob);
                    }

                    /*! API mapping of the %CAN driver status */
                    DWORD status() {
                        return CAN_Status(handle);
                    }

            };

            /*! define a generic type from where the PeakDriver could be
             *  derived.
             */
            typedef PeakDriverLinuxTrait PeakDriverTrait;

        } /* namespace CAN */
    } /* namespace nic */
} /* namespace device */

#endif

