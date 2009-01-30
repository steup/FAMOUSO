#ifndef __avrCANARY_h__
#define __avrCANARY_h__

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>

#include "mw/nl/can/canID.h"
#include "mw/nl/can/GenericReceiveInterrupt.h"

namespace device {
    namespace nic {
        namespace CAN {

            /*! \file avrCANARY.h
             *
             * \class avrCANARY
             *
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
             * \todo needs further documentation
             *
             */
            class avrCANARY {
                public:

                    /*! \class MOB
                     *
                     * \brief Generic CAN message object
                     *
                     * \todo needs unification with the MOB of the peak CAN driver
                     *       and further development to cover the SocketCAN driver
                     *       as well. Maybe is a comman base class or a template
                     *       specializeble class possible.
                     */
                    class MOB {
                        public:
                            typedef famouso::mw::nl::CAN::CANARY::ID  IDType;
                            IDType  _id;
                            uint8_t _data[8];
                            uint8_t _len;

                            void extended() { }

                            IDType& id() {
                                return *reinterpret_cast<IDType*>(&_id);
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

                    avrCANARY() {}

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

                    void receive_blocking(MOB *mob) {
                        while (!receive(mob));
                    }

                    void send(MOB &mob) {
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
                            CANMSG = mob.data() [i];

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
//                    famouso::util::Delegate<> tx_Interrupt;


                    void interrupts_on() {
                        CANGIE = (1 << ENIT) | (1 << ENRX); //enable receive interrupt
                    }

                    void interrupts_off() {
                        CANGIE = 0;
                    }

            };
        } /* namespace CAN */
    } /* namespace nic */
} /* namespace device */

SIGNAL (SIG_CAN_INTERRUPT1) {
    device::nic::CAN::detail::rx_Interrupt();
}

#endif
