/*******************************************************************************
 *
 * Copyright (c) 2010 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
 *               2010 Karl Fessel <karl.fessel@st.ovgu.de>
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
#ifndef __ATMEL_SERIAL_DRIVER_h__
#define __ATMEL_SERIAL_DRIVER_h__

#include "avr-halib/share/cframe.h"
#include "avr-halib/share/delegate.h"

#include "case/Delegate.h"
#include "devices/nic/serial/SerialFrameDelimiter.h"
#include "debug.h"

using namespace std;

namespace device {
    namespace nic {
        namespace serial {
            /*! \class  AtmelSerialDriver AtmelSerialDriver.h "device/nic/serial/AtmelSerialDriver.h"
             *  \brief  %Atmel %driver for the serial network layer.
             *
             *  \tparam character %device used for the serial connection
             *  \tparam available payload of the %driver (255 byte)
             *  \tparam baudrate used by the %driver (default 19200 bps)
             *  \tparam byte modifier for framing/stuffing
             *
             *  \todo minimize number of delegates used
             *  \todo move implementation of CFrame to FAMOUSO
             *  \todo use same framing/stuffing implementation as the AsioDriver
             */
            template <
                        class DEV,
                        uint8_t PL  = 255,
                        uint16_t BR = 19200,
                        class SFD   = struct SerialFrameDelimiter
                     >
            class AtmelSerialDriver : public CFrame< DEV, uint8_t, SFD, PL> {
                public:
                    /*! \brief  definition of base type*/
                    typedef CFrame< DEV, uint8_t, SFD, PL> baseType;
                    /*! \brief  definition of class type*/
                    typedef AtmelSerialDriver<DEV, PL, BR, SFD> type;
                    /*! \brief  definition of driver specific message object*/
                    typedef typename baseType::mob_t mob_t;
                    /*! \brief  definition of driver specific information*/
                    struct cfgInfo {
                        enum {
                            payload = PL
                        };
                    };
                protected:
                    void handleReceive() {
                        if (this->onReceive) this->onReceive();
                    }
                    void handleReady()   {
                        if (this->onReady) this->onReady();
                    }
                public:
                    AtmelSerialDriver() {}
                    ~AtmelSerialDriver() {}

                    /*! \brief  (Re)Initializes the %driver for the serial
                     *          connection.
                     */
                    void init() {
                        TRACE_FUNCTION;
                        baseType::onReady.template bind< type, &type::handleReady>(this);
                        baseType::onReceive.template bind<type, &type::handleReceive>(this);
                        baseType::enableonReceive();
                    }

                    /*! \brief  Fetches the message received last.
                     *  \param[out] message
                     */
                    void recv(mob_t& message) {
                        TRACE_FUNCTION;
                        baseType::recv(message);
                    }

                    /*! \brief  Transmits a message using the serial port.
                     *  \param[in] mob message %object that is to be send
                     */
                    void send(const mob_t& message) {
                        TRACE_FUNCTION;
                        baseType::send(message);
                    }

                    /*! \brief  Triggered if a packet was received.*/
                    famouso::util::Delegate<> onReceive;

                    /*! \brief  Triggered if %driver is able to send new/further
                     *          messages.
                     *  \note   This functionality is not yet implemented.
                     *  \todo   provide functionality for the onReady() interrupt
                     */
                    famouso::util::Delegate<> onReady;
            };
        } /* namespace armarow */
    } /* namespace nic */
} /* namespace device */

#endif  //__ATMEL_SERIAL_DRIVER_h__
