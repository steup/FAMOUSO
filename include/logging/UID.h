/*******************************************************************************
 *
 * Copyright (c) 2011 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __LOG_UID_H_71B9209296A88B__
#define __LOG_UID_H_71B9209296A88B__

#include "debug.h"
#include "mw/common/UID.h"


/*! \brief Operator to output a UID.
 *
 * \param out The output stream to print to.
 * \param uid The UID to print.
 * \return The output stream for chaining calls.
 */
inline ::logging::loggingReturnType &operator <<(::logging::loggingReturnType &out, const UID &uid) {
    bool printable = true;
    for (uint8_t i = 0; i < 8; ++i) {
        uint8_t c = uid.tab()[i];
        if ((c < 32) || (c > 126)) {
            printable = false;
            break;
        }
    }
    out << '(';
    if (printable) {
        // Print as string, if printable
        out << '\"';
        for (uint8_t i = 0; i < 8; ++i) {
            out << uid.tab()[i];
        }
        out << '\"';
    } else {
        // Print hex if it contains non-printable characters
        out << ::logging::log::hex << uid.value();
    }
    out << ')';
    return out;
}


#endif // __LOG_UID_H_71B9209296A88B__

