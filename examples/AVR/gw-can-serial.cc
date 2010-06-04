/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
 *                    2010 Karl Fessel <karl.fessel@st.ovgu.de>
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
/*! \file   examples/AVR/gw-can-serial.cc
 *  \brief  Example gateway beween an AVR serial and a CAN Network Layer.
 */
#ifdef __AVR_AT90CAN128__
#define CPU_FREQUENCY 16000000UL
#else
#define CPU_FREQUENCY 8000000UL
#endif
#define F_CPU CPU_FREQUENCY
#define USE_PHY_CLASS
#define UARTLOGDEVICE Uart0
/* === includes ============================================================= */
#include <util/delay.h>
//-------------------------------------------------------------------
// includes for AVR serial network layer
//-------------------------------------------------------------------
#include "avr-halib/avr/uart.h"
#include "avr-halib/share/delay.h"
#include "devices/nic/serial/AtmelSerialDriver.h"
#include "mw/nl/SerialNL.h"
//-------------------------------------------------------------------
// includes for CAN network layer
//-------------------------------------------------------------------
#include "devices/nic/can/at90can/avrCANARY.h"
#include "mw/common/UID.h"
#include "mw/nl/CANNL.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/nl/can/canID.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"
//-------------------------------------------------------------------
// includes for common famouso middlware
//-------------------------------------------------------------------
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/nal/NetworkAdapter.h"
#include "mw/gwl/Gateway.h"
#include "mw/el/EventLayer.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"
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
    namespace GW_CAN_SERIAL {
        class config {
            protected:
                // CAN-NL specific typedefinitions
                typedef device::nic::CAN::avrCANARY can;
                typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
                typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
                typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> nlc;
                typedef famouso::mw::anl::AbstractNetworkLayer< nlc > anlc;
                // Serial-NL specific typedefinitions
                struct UCFG : public Uart1<RBoardController> {
                    enum { baudrate = 19200 };
                };
                typedef Uart< UCFG > dev_t;
                typedef device::nic::serial::AtmelSerialDriver< dev_t, 128, 19200> drv;
                typedef famouso::mw::nl::SerialNL< drv > nls;
                typedef famouso::mw::anl::AbstractNetworkLayer< nls > anls;
                typedef famouso::mw::nal::NetworkAdapter< anlc, anls > na;
            public:
                typedef famouso::mw::el::EventLayer< na > EL;
                typedef famouso::mw::gwl::Gateway<EL> GW;
                typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
                typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
        };
    }
    typedef GW_CAN_SERIAL::config config;
}
/* === main ================================================================= */
int main() {
    sei();                              // enable interrupts
    ::logging::log::emit()
        << "Starting serial-can gateway example!"
        << ::logging::log::endl << ::logging::log::endl;

    famouso::init<famouso::config>();   // initialize famouso
    ::logging::log::emit()
        << "FAMOUSO -- Initalisation successfull" << ::logging::log::endl;
    Idler::idle();                      // duty cycle
}
