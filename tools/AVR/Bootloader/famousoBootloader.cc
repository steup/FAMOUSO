/*******************************************************************************
 *
 * Copyright (c) 2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
 * $Id: $
 *
 ******************************************************************************/

#define F_CPU   16000000

#include "debug.h"
LOGGING_DISABLE_LEVEL(::logging::Error);
LOGGING_DISABLE_LEVEL(::logging::Warning);
#include "Board.h"
#include "Application.h"
#include "bootloader.h"

#define EVENT_LENGTH 1024

#include <avr/io.h>

#include "famouso.h"
#include "mw/anl/AbstractNetworkLayer.h"
#include "mw/el/EventLayer.h"
#include "mw/api/PublisherEventChannel.h"
#include "mw/api/SubscriberEventChannel.h"
#include "mw/nl/CANNL.h"
#include "devices/nic/can/at90can/avrCANARY.h"
#include "mw/nl/can/etagBP/Client.h"
#include "mw/nl/can/ccp/Client.h"
#include "mw/afp/Config.h"

struct MinAFPConfig : famouso::mw::afp::DefaultConfig {
    typedef object::OneBlockAllocator<famouso::mw::afp::EmptyType, EVENT_LENGTH> Allocator;
    typedef famouso::mw::afp::DefaultEventSizeProp SizeProperties;
    enum {
        overflow_error_checking = false
    };
};

namespace famouso {
    namespace CAN {
        class config {
                typedef device::nic::CAN::avrCANARY can;
                typedef famouso::mw::nl::CAN::ccp::Client<can> ccpClient;
                typedef famouso::mw::nl::CAN::etagBP::Client<can> etagClient;
                typedef famouso::mw::nl::CANNL<can, ccpClient, etagClient> nl;
                typedef famouso::mw::anl::AbstractNetworkLayer<
                            nl,
                            famouso::mw::afp::NoFragmentation,
                            MinAFPConfig
                        > anl;
            public:
                typedef famouso::mw::el::EventLayer< anl > EL;
                typedef famouso::mw::api::PublisherEventChannel<EL> PEC;
                typedef famouso::mw::api::SubscriberEventChannel<EL> SEC;
        };
    }

    typedef CAN::config config;
}

uint32_t page;

void ReceiveCallback(famouso::mw::api::SECCallBackData& cbd) {
   ::logging::log::emit() << "FS " << ::logging::log::endl;
   boot_program_page(page,cbd.data);
   page+=Board<BOARD>::flashPageSize;
   ::logging::log::emit() << "FE " << ::logging::log::endl;
}

void bootloader() __attribute__((noreturn));
void bootloader() {
        Board<BOARD>::enableInterrupt();

        ::logging::log::emit() << "Bootloader started " << ::logging::log::endl;
        Board<BOARD>::signalize_bootloader_active();

        famouso::init<famouso::config>();

        famouso::config::SEC sec("UPDATEuC");

        sec.subscribe();
        sec.callback.bind<ReceiveCallback>();
        ::logging::log::emit() << "Bootloader started" << ::logging::log::endl;
        while(1);

}

void start_condition() __attribute__((section(".init0"), naked));
void start_condition() {
    if (!Board<BOARD>::start_condition())
        __asm__ __volatile (
            "jmp 0" : : : );
}

int main(void) {
    bootloader();
}
