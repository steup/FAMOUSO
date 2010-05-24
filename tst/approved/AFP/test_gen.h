/*******************************************************************************
*
* Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
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


#ifndef __TEST_H_D2A4F69A9B065C__
#define __TEST_H_D2A4F69A9B065C__


#include <stdint.h>
#include "util/endianness.h"


/*!
 *  \brief  Creates event content that can be check by check_event_content()
 *  \param length   Length of the event
 *  \param data     Pointer to data buffer of the event
 */
void create_event_content(uint32_t length, uint8_t * data) {
    // Special case... very small event
    if (length <= sizeof(uint32_t)) {
        for (uint32_t i = 0; i < length; i++)
            data[i] = (uint8_t)i;
        return;
    }

    // Save length at the beginning
    *((uint32_t *)data) = htonl(length);

    // Fill rest with pattern
    for (uint32_t i = sizeof(uint32_t); i < length; i++)
        data[i] = (uint8_t)i;
}

/*!
 *  \brief  Checks whether event content is equal to that created by create_event_content()
 *  \param length   Length of the event
 *  \param data     Pointer to data buffer of the event
 *  \return True if content is correct
 */
bool check_event_content(uint32_t length, uint8_t * data) {
    // Special case... very small event
    if (length <= sizeof(uint32_t)) {
        for (uint32_t i = 0; i < length; i++)
            if (data[i] != i)
                return false;
        return true;
    }

    // Read length
    uint32_t l = ntohl(*((uint32_t *)data));
    if (length != l)
        return false;

    // Check the rest of the message
    for (uint32_t i = sizeof(uint32_t); i < length; i++)
        if (data[i] != (uint8_t)i)
            return false;

    return true;
}





#endif // __TEST_H_D2A4F69A9B065C__

