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

#ifndef __SimulateInterruptViaThreadAdapter_h__
#define __SimulateInterruptViaThreadAdapter_h__

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "devices/nic/can/PAXCAN/RxInterrupt.h"

namespace device {
    namespace nic {
        namespace CAN {

            /*! \brief This class enable asynchronous notification by simulating
             *         an interrupt with a thread, that block on a resource.
             *         If the resource gets ready, it calls a delegate for
             *         serving the asynchronous action like an interrupt.
             *
             *  \tparam Base is the class/type for which the additional interrupt
             *          functionality is provides
             *  \tparam InterruptProvider offers the interface and functionality
             *          of interrupts and the SimulateInterruptViaThreadAdapter
             *          calls a generic interrupt fire method if the ressource
             *          gets ready.
             */
            template < class Base, class InterruptProvider = RxInterrupt >
            class SimulateInterruptViaThreadAdapter : public Base, public InterruptProvider {
                public:
                    /*! \brief initialize the base and activates the service thread */
                    void init() {
                        Base::init();
                        sim = new boost::thread(
                            boost::bind(&SimulateInterruptViaThreadAdapter::simulatedInterruptViaThread,
                                        this));
                    }

                private:

                    /*! \brief the method of the servicing thread for receiving
                     *         messages allowing asynchonity and callbacking
                     *         for simulating receive interrupts
                     */
                    void simulatedInterruptViaThread() {
                        typename Base::MOB mob;
                        while (1) {
                            if (Base::interrupt_condition ()) {
                                /*! \todo the interrupt has to be synchronized
                                 *        with the asio/other thread in order
                                 *        to avoid inconsistencies in the
                                 *        data-structures of the EventLayer
                                 */
                                InterruptProvider::fire_Interrupt();
                            } else {
                                // sleep a short while
                            }
                        }
                    }
                    /*! \brief Thread that does the asynchronous reading of CAN messages */
                    boost::thread *sim;
            };

        } /* namespace CAN */
    } /* namespace nic */
} /* namespace device */

#endif

