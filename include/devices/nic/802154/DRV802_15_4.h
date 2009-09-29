/*******************************************************************************
 *
 * Copyright (c) 2008, 2009
 * Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 * Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
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

#ifndef __DRV802_15_4_h__
#define __DRV802_15_4_h__

#include "case/Delegate.h"
#include "debug.h"

namespace device {
    namespace nic {
        namespace ieee802_15_4 {

            /*! \class  DRV802_15_4
             *  \brief  Communication driver for communication layers provided by the 'RFLayer'.
             *
             *  This driver uses the communication layer for wireless communication provided
             *  by the 'RFLayer' to transmit and receive messages.
             *
             *  \param[in]  RFLayer  wireless communication layer the driver uses
             *
             *  \todo standard error handling
             *  \todo use actual size of availeble payload during compile time
             *  \todo use MOB as provided by the communication layer (template parameter)
             */
            template <class RFLayer>
            class DRV802_15_4 {
                public:
                    /*! \brief  definition of class type including template parameters */
                    typedef DRV802_15_4 type;
                    /*! \brief  definition of message object provided by the communication layer */
                    typedef wsn::dll::mac::MACReceiveData  mob_t;
                private:
                    RFLayer _rflayer;

                    /*! \brief  Callback method triggered after a packet was successfull received.
                     *
                     *  \todo   use of better syntax to determin if the Delegate used is enabled
                     */
                    void rfReceiveCallback(const mob_t& rxData) {
                        // trigger interrupt after receiving data
                        //TODO if ( !(rx_Interrupt.isEmpty()) ) {
                        if ( rx_Interrupt ) {
                            rx_Interrupt(rxData);
                        }
                    }

                    /*! \brief  Callback method triggered if an error occured during communication.
                     */
                    void rfErrorCallback(const wsn::dll::mac::ErrorData<uint8_t>& errData) {
                        //TODO need there be an action if the error callback is triggered?
                    }

                public:
                    DRV802_15_4() {}
                    ~DRV802_15_4() {}

                    /*! \brief  Initialization method for the driver.
                     *
                     *  During the initialization the callback methods are registered and
                     *  the lower layers of communication are initialized.
                     */
                    void init() {
                        // Register our receive callback at the MAC layer object
                        _rflayer.rx_Interrupt.template bind<type, &type::rfReceiveCallback>(this);
                        // Register our error callback at the MAC layer object
                        _rflayer.error_Interrupt.template bind<type, &type::rfErrorCallback>(this);

                        _rflayer.init(SHORT_ADDR, PAN_ID, COMM_CHANNEL);
                    }

                    /*! \brief Transmitts a message using the communication layer provided as template parameter.
                     *
                     *  \param[in]  buffer pointer to data to be send
                     *  \param[in]  size   size of data to be send
                     *
                     *  \todo       determin the destination adress/route if message is not broadcasted
                     *  \todo       use enum parameter for sanity check of maximal payload
                     */
                    void send(const uint8_t* buffer, uint8_t size) {
                        ::logging::log::emit< ::logging::Trace>() << FUNCTION_SIGNATURE << ::logging::log::endl;
                        // asserts if data size corresponds to payload size of communication layer
                        if ( size > _rflayer.getMaxPayload() ) {
                            ::logging::log::emit< ::logging::Error>()
                                    <<"Payload size violation sending DATA RF"
                                    << ::logging::log::endl;
                            return;
                        }

                        // transmit buffer
                        if (!(_rflayer.send(buffer, size, DESTINATION))) {
                            ::logging::log::emit< ::logging::Error>()
                                    <<"MAC transmission failed!"
                                    << ::logging::log::endl;
                            //TODO need there be an error response if transmission failed?
                        }
                    }

                    /*! \brief  The rx_Interrupt is called after a packet was successfully received.
                      */
                    famouso::util::Delegate<const mob_t&> rx_Interrupt;

                    /*! \brief  The tx_Interrupt is called if the driver is able to send
                     *          new/further messages. (not yet implemented)
                     *
                     *  \todo   Provide functionality of using tx_interrupt
                     */
                    famouso::util::Delegate<> tx_Interrupt;
            };
        } /* namespace ieee802_15_4 */
    } /* namespace nic */
} /* namespace device */

#endif

