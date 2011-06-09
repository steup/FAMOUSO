/*******************************************************************************
 *
 * Copyright (c) 2011 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __GP2D120_H_B40F8BFA2A02EA__
#define __GP2D120_H_B40F8BFA2A02EA__

#include <avr/pgmspace.h>

/* Lookup table for 8-bit ADC to disctance in centimeters based on
 * experimentation with AREF=3.3V.  See www.micahcarrick.com for more details.
*/

static uint8_t __attribute__((section(".progmem.gp2d120_data")))
    gp2d120_data[255] = {
        // 0
        30,30,30,30,30,30,30,30,30,30,
        30,30,30,30,30,30,30,30,30,30,
        30,30,30,30,30,30,30,30,30,30,
        30,30,30,30,30,30,29,29,29,28,
        28,28,27,27,27,26,26,26,25,25,
        25,24,24,24,23,23,22,22,22,21,
        21,21,20,20,20,19,19,19,19,19,
        18,18,18,18,18,17,17,17,17,16,
        16,16,16,16,16,16,15,15,15,15,
        15,15,14,14,14,14,14,14,13,13,
        // 100
        13,13,13,13,13,13,13,12,12,12,
        12,12,12,12,12,12,12,11,11,11,
        11,11,11,11,11,11,11,11,11,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10, 9, 9, 9, 9, 9, 9, 9, 9,
         9, 9, 9, 9, 9, 9, 9, 9, 9, 8,
         8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
         8, 8, 8, 8, 8, 8, 8, 8, 8, 7,
         7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
         7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
         // 200
         7, 6, 6, 6, 6, 6, 6, 6, 6, 6,
         6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
         6, 6, 6, 6, 5, 5, 5, 5, 5, 5,
         5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
         3, 3, 3, 3, 3
};

/* Returns a uint8_t value representing the cm to the object based on the 8-bit
 * ADC values passed to the function.  Uses the lookup table values.
*/
uint8_t gp2d120_adc8_to_cm (uint8_t adc_value) {
        return pgm_read_byte(&gp2d120_data[adc_value]);
}

#endif // __GP2D120_H_B40F8BFA2A02EA__
