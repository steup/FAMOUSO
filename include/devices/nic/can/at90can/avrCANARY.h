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

#ifndef __avrCANARY_h__
#define __avrCANARY_h__

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>

#include "mw/nl/can/canID_LE_AVR.h"
#include "case/Delegate.h"

namespace device {
    namespace nic {
        namespace CAN {

            /*!
             * \brief The generic driver interface to the avrCAN hardware.
             *
             * At the moment only a basic and very simple CAN driver is
             * implemented. It uses only one message object for receiving
             * and one for sending. The receiving of Mobs is interrupt
             * based and the sending is polling based at the moment. In
             * further more enhanced version the interface may evolve to
             * support and use more features of the hardware. But this
             * version is the working proof of concept.
             *
             * \todo Use the supported features of the avr CAN hardware
             *       e.g. more message objects, transmit interrupt, ...
             *
             */
            class avrCANARY {
                public:

                    /*! \class MOB
                     *
                     * \brief Generic CAN message object
                     */
                    class MOB {
                        public:
                            typedef famouso::mw::nl::CAN::detail::ID<
                                        famouso::mw::nl::CAN::detail::famouso_CAN_ID_LE_CANARY
                                    > IDType;
                            IDType  _id;
                            uint8_t _data[8];
                            uint8_t _len;

                            void extended() { }

                            IDType& id() {
                                return _id;
                            }

                            uint8_t& len() {
                                return _len;
                            }

                            void len(uint8_t l) {
                                _len = l;
                            }

                            uint8_t *data() {
                                return _data;
                            }

                            void data(uint8_t *) {}

                            uint8_t &data(uint8_t i) {
                                return _data[i];
                            }
                    };

                    explicit avrCANARY() {}

                    /*! \brief receives a %CAN message in a blocking manner
                     *
                     *  \param[out] mob is filled with the arrived message if there
                     *              was one
                     */
                    bool receive(MOB* mob) {
                        // save CANPAGE
                        register uint8_t temp = CANPAGE;
                        bool ret = false;

                        // select Mob0
                        CANPAGE = 0x00;
                        if ((CANSTMOB&(1 << RXOK))) {
                            // clear interrupt flag
                            CANSTMOB &= ~(1 << RXOK);
                            // get identifier
                            mob->id()[3] = CANIDT1;
                            mob->id()[2] = CANIDT2;
                            mob->id()[1] = CANIDT3;
                            mob->id()[0] = CANIDT4 & 0xF8;

                            // get data
                            for (uint8_t i = 0; i < (CANCDMOB&0xf); i++)
                                mob->data()[i] = CANMSG;

                            // reconfig mailbox
                            CANSTMOB = 0;
                            CANCDMOB = (1 << CONMOB1) | (1 << IDE);
                            ret = true;
                        }
                        //restore CANPAGE
                        CANPAGE = temp;
                        return ret;

                    }

                    /*! \brief receives a %CAN message in a blocking manner
                     *
                     *  \param[out] mob is filled with the arrived message if there
                     *              was one
                     */
                    void receive_blocking(MOB *mob) {
                        while (!receive(mob));
                    }

                    /*! \brief transmit a %CAN message in a blocking manner
                     *
                     *  \param[in] mob is %CAN message that will be delivered
                     */
                    void transmit(MOB &mob) {
                        //enable MOb1, auto increment index, start with index = 0
                        CANPAGE = (1 << 4);

                        //set IDE bit, length = 8
                        CANCDMOB = (1 << IDE) | (mob.len() << DLC0);

                        CANIDT4 = mob.id()[0] & 0xF8;
                        CANIDT3 = mob.id()[1];
                        CANIDT2 = mob.id()[2];
                        CANIDT1 = mob.id()[3];

                        // put data in mailbox
                        for (unsigned char i = 0; i < mob.len(); i++)
                            CANMSG = mob.data()[i];

                        // enable transmission
                        CANCDMOB |= (1 << CONMOB0);
                        // wait until transmission complete
                        while (!(CANSTMOB & (1 << TXOK)));
                        // reset flag
                        CANSTMOB &= ~(1 << TXOK);

                    }

                    /*******************************************************************
                      CAN initialization:
                      Mailbox 0: Receive  --> interrupt
                      Mailbox 1: Transmit --> polling
                    *******************************************************************/
                    void init() {
                        // reset CAN interface
                        CANGCON |= (1 << SWRES);

                        // reset all MObs
                        for (unsigned char i = 0; i < 15; i++) {
                            CANPAGE = (i << 4); // select MOb
                            CANCDMOB = 0;       // disable MOb
                            CANSTMOB = 0;       // clear status
                            CANIDT1 = 0;        // clear ID
                            CANIDT2 = 0;
                            CANIDT3 = 0;
                            CANIDT4 = 0;
                            CANIDM1 = 0;        // clear mask
                            CANIDM2 = 0;
                            CANIDM3 = 0;
                            CANIDM4 = 0;
                            for (unsigned char j = 0; j < 8; j++)
                                CANMSG = 0;     // clear data
                        }

                        // bit timing -> datasheet 264 (check table)
                        // 250Kbps 16MHz cpu-clk
                        CANBT1 = 0x0E;
                        CANBT2 = 0x04;
                        CANBT3 = 0x13;

                        // start CAN interface
                        CANGCON = (1 << ENASTB);

                        // configure MOb0
                        CANPAGE = (0 << 4);
                        /*      CANIDT1 = 0;                    //ID
                                CANIDT2 = 0;
                                CANIDT3 = 0;
                                CANIDT4 = 0;
                                CANIDM1 = 0;    // get all messages
                                CANIDM2 = 0;    // 1 = check bit
                                CANIDM3 = 0;    // 0 = ignore bit
                                CANIDM4 = (1<<IDEMSK);
                        */
                        CANIDM4 |= (1 << IDEMSK);
                        CANSTMOB = 0;
                        // reception, ext. ID
                        CANCDMOB = (1 << CONMOB1) | (1 << IDE);

                        //interrupts
                        CANIE1 = 0;
                        // enable MOb0 interrupt
                        CANIE2 = (1 << 0);
                        CANSIT1 = 0;
                        CANSIT2 = 0;

                        // wait until module ready
                        while (!(CANGSTA & (1 << ENFG)));
                    }

                    /*! \brief set the delegate that is called if an interrupt occurs */
                    void set_rx_Interrupt(famouso::util::Delegate<> f) {
                        rx_Interrupt = f;
                    }


                    /*! \brief switch interrupts for the %device on or off
                     */
                    void rx_interrupts(bool value) {
                        if (value)
                            CANGIE = (1 << ENIT) | (1 << ENRX); //enable receive interrupt
                        else
                            CANGIE = 0;
                    }

                    /*! \brief get the current state of the interrupt switch for the %device
                     */
                    bool interrupts() {
                        return CANGIE & ((1 << ENIT) | (1 << ENRX));
                    }

                private:
                    /*! \brief the receive interrupt delegate allow binding of
                     *         ordinary C-functions as well as C++ class methods
                     */
                    static famouso::util::Delegate<> rx_Interrupt;

                    friend void device::nic::CAN::fireCANARYInterrupt();
            };

            famouso::util::Delegate<> avrCANARY::rx_Interrupt;

            void inline fireCANARYInterrupt() {
                device::nic::CAN::avrCANARY::rx_Interrupt();
            }

        } /* namespace CAN */
    } /* namespace nic */
} /* namespace device */

namespace {
    SIGNAL(SIG_CAN_INTERRUPT1) {
        device::nic::CAN::fireCANARYInterrupt();
    }
}

#endif
