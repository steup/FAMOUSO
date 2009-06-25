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

#ifndef __PAXCAN_h__
#define __PAXCAN_h__

#include "object/SynchronizedBoundedBuffer.h"

namespace device {
    namespace nic {
        namespace CAN {

            /*! \brief  PAXCAN presents the generic %CAN interface to the FAMOUSO
             *          middleware, against all algorithms regarding %CAN transfers
             *          are implemented.
             *
             *  \tparam the Driver is the underlying %CAN driver, that could be a
             *          SocketCANDriver, PeakDriver or something else that provide
             *          a specific interface.
             */
            template <class Driver>
            class PAXCAN : public Driver {
                public:

                    PAXCAN() : sbb(1000) {}

                    /*! \brief initialize the subclasses */
                    void init() {
                        Driver::init();
                    }

                    /*! \brief receives a %CAN message
                     *
                     *  \param[out] mob is filled with the arrived message if there
                     *              was one
                     *
                     *  \return true if a %CAN message was arrived
                     *  \return false otherwise
                     */
                    bool receive(typename Driver::MOB* mob) {
                        if (sbb.is_not_empty()) {
                            sbb.pop_back(mob);
                            return true;
                        } else {
                            return false;
                        }
                    }

                    /*! \brief receives a %CAN message in a blocking manner
                     *
                     *  \param[out] mob is filled with the arrived message if there
                     *              was one
                     */
                    void receive_blocking(typename Driver::MOB *mob) {
                        sbb.pop_back(mob);
                    }

                    /*! \brief sends a %CAN message in a blocking manner
                     *
                     *  \param[in] mob is %CAN message that will be delivered
                     */
                    void send(typename Driver::MOB &mob) {
                        Driver::write(mob);
                    }

                private:
                    /*! \brief Used for storing messages in a thread safe manner */
                    object::SynchronizedBoundedBuffer<typename Driver::MOB >  sbb;

                protected:
                    /*! \brief test if there is a message arrived and signalise the
                     *         calling context, whether the interrupt condidtion is
                     *         true of false
                     *
                     *  \return true if a %CAN message was arrived
                     *  \return false otherwise
                     */
                    bool interrupt_condition () {
                        typename Driver::MOB mob;
                        if (Driver::read(mob) == true ) {
                            sbb.push_front(mob);
                            return true;
                        }
                        return false;
                    }
            };

        } /* namespace CAN */
    } /* namespace nic */
} /* namespace device */

#endif

