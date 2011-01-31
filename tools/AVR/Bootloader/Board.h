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

#ifndef __BOARD_H_EDEDED9B52D8AD__
#define __BOARD_H_EDEDED9B52D8AD__


#ifndef BOARD
#define BOARD int
#else
struct BOARD;
#endif

#include "BaseAVR.h"

#include <util/delay.h>

struct chip45;

template <typename T>
struct Board {
    static bool start_condition () {
        int you_did_not_select_a_supported_board_thus_the_start_condition_will_always_be_false;
        return false;
    }

    static void signalize_bootloader_active() {}
    static void enableInterrupt() {}
    static void writeFlashPage(uint32_t page, uint8_t *buf) __attribute__ ((always_inline)) {}
};

template <>
struct Board<chip45> : BaseAVR {

    static bool start_condition () __attribute__ ((always_inline)) {
	    DDRA  &= ~(1<<PINA7);		// set as Input
	    PORTA |= (1<<PINA7);		// Enable pullup
        return !(PINA & (1<<PINA7));
    }

    static void signalize_bootloader_active() __attribute__ ((always_inline)) {
        int16_t i=0xff;
        do {
            DDRB^=(1<<7);
            do {
                _delay_ms(25);
            } while (--i & 0xf);
        } while(--i>0);
    }

    static const uint16_t   bootSize=4096;
    static const uint16_t   flashPageSize=SPM_PAGESIZE;
    static const uint32_t   maxAppEnd=(FLASHEND - (bootSize * 2));
    static const uint64_t   fCPU=16000000;
};

struct Experiment;
template <>
struct Board<Experiment> : Board<chip45> {

    static bool start_condition () __attribute__ ((always_inline)) {
	    DDRB  &= ~(1<<PINB2);		// set as Input
	    PORTB |= (1<<PINB2);		// Enable pullup
        return !(PINB & (1<<PINB2));
    }
};

#endif // __BOARD_H_EDEDED9B52D8AD__
