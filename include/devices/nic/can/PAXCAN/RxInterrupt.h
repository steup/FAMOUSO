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

#ifndef __RxInterrupt_h__
#define __RxInterrupt_h__

#include "case/Delegate.h"

namespace device {
    namespace nic {
        namespace CAN {

            /*! \brief  The RxInterrupt enables receive interrupt service base infrastructur
             *          for devices that are not able to generate such interrupts. It defines
             *          a handler and the functionality for switching on/off of the interrupt.
             *
             *          In order to fire interrupts it needs additional functionality like
             *          device::nic::CAN::SimulateInterruptViaThreadAdapter
             */
            class RxInterrupt {
                public:
                    /*! \brief set the delegate that is called if an interrupt occurs */
                    void set_rx_Interrupt(famouso::util::Delegate<> f) {
                        rx_Interrupt = f;
                    }

                    /*! \brief switch interrupts for the %device on or off
                     */
                    void rx_interrupts(bool value) {
                        ints_allowed = value;
                    }

                    /*! \brief get the current state of the interrupt switch for the %device
                     */
                    bool interrupts() {
                        return ints_allowed;
                    }

                    /*! \brief fires the interrupt if interrupt is set and allowed */
                    void fire_Interrupt() {
                        if (rx_Interrupt && interrupts()) rx_Interrupt();
                    }

                private:
                    /*! \brief the receive interrupt delegate allow binding of
                     *         ordinary C-functions as well as C++ class methods
                     */
                    famouso::util::Delegate<> rx_Interrupt;

                    /*! \brief member variable the indicates if interrupts are allowed or
                     *         not.
                     */
                    bool ints_allowed;
            };

        } /* namespace CAN */
    } /* namespace nic */
} /* namespace device */

#endif

