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

#ifndef __PeakDriver_h__
#define __PeakDriver_h__

#include "debug.h"
#include <stdint.h>

namespace device {
    namespace nic {
        namespace CAN {

            /*!\brief The generic driver interface to the PeakCAN hardware.
             *
             * This generic driver for the PeakCAN hardware allows transmitting
             * and receiving of %CAN messages. It has to configured via a trait
             * the defines the peculiarity of the underling platform (Linux or
             * Windows)
             */
            template < class PeakTrait >
            class PeakDriver : public PeakTrait {
                public:

                    /*! inialize the driver and it subobjects */
                    void init() {
                        PeakTrait::init();
                    }

                    /*! transmits a %CAN message %object
                     *
                     * \param[in] mob that has to be transmitted
                     */
                    void write(typename PeakTrait::MOB &mob) {
                        if ((errno = PeakTrait::write_mob(mob))) {
                            ::logging::log::emit< ::logging::Error>()
                                << "CAN_Read() error" << ::logging::log::endl
                                << "closing application" << ::logging::log::endl;
                            exit(0);
                        }
                    }

                    /*! try receiving a %CAN message %object
                     *
                     * \param[out] mob that will befilled with a received message
                     *
                     * \return true if, a message was received
                     * \return false otherwise
                     */
                    bool read(typename PeakTrait::MOB &mob) {
                        uint32_t status;
                        errno = PeakTrait::read_mob(mob);
                        switch (errno) {
                            case    CAN_ERR_OK : {
                                    if (mob.MSGTYPE & MSGTYPE_STATUS) {
                                        status = PeakTrait::status();
                                        if ((int)status < 0) {
                                            ::logging::log::emit< ::logging::Error>()
                                                << "CAN error" << ::logging::log::endl
                                                << "closing application" << ::logging::log::endl;
                                            exit(0);
                                        } else
                                            ::logging::log::emit< ::logging::Error>()
                                                << "pending %CAN status 0x." << status
                                                << " read." << ::logging::log::endl;
                                        return false;
                                    }
                                    return true;
                                }
                            case    CAN_ERR_QRCVEMPTY: break; // hier muss warten implementiert werden, falls zu oft gepolled wird
                            default: {
                                    ::logging::log::emit< ::logging::Error>()
                                        << "CAN_Read() error" << ::logging::log::endl
                                        << "closing application" << ::logging::log::endl;
                                    exit(0);
                                }
                        }
                        return false;
                    }
            };

        } /* namespace CAN */
    } /* namespace nic */
} /* namespace device */

#endif

