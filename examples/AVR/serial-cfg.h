/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
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
#ifndef __AVR_EXAMPLE_SERIAL_h__
#define __AVR_EXAMPLE_SERIAL_h__
/*! \brief  Configuration for an AVR Serial Network Layer.
 *  \file   examples/AVR-NetworkLayer/serial-cfg.h
 */
#define UARTLOGDEVICE Uart0
/* === includes ============================================================= */
//-------------------------------------------------------------------
// includes for serial network layer
//-------------------------------------------------------------------
#include "avr-halib/avr/uart.h"
#include "avr-halib/share/delay.h"
#include "devices/nic/serial/AtmelSerialDriver.h"
#include "mw/nl/SerialNL.h"
//-------------------------------------------------------------------
// includes for common famouso middlware
//-------------------------------------------------------------------
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"
#include "mw/common/Event.h"
#include "mw/el/EventLayer.h"
#include "util/Idler.h"

#include "famouso.h"
/* === macros =============================================================== */
UseInterrupt(SIG_USART1_RECV);
UseInterrupt(SIG_USART1_DATA);
/* === types ================================================================ */
struct RBoardController {               // configuration of hardware platform
    enum {
#ifdef __AVR_ATmega128__
        controllerClk = 8000000
#else
        controllerClk = 16000000
#endif
    };
};

namespace famouso {
    namespace serial {
        class config {
            struct UCFG : public Uart1<RBoardController> {
                    enum { baudrate = 19200 };
                };
                typedef Uart< UCFG > dev_t;
                typedef device::nic::serial::AtmelSerialDriver< dev_t, 128, 19200> drv_t;
                typedef famouso::mw::nl::SerialNL< drv_t > nl;
                typedef famouso::mw::anl::AbstractNetworkLayer< nl > anl;
            public:
                typedef famouso::mw::el::EventLayer< anl > EL;
                typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
                typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
        };
    }
    typedef serial::config config;
}

#endif  //__AVR_EXAMPLE_SERIAL_h__
