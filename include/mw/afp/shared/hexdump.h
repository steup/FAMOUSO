/*******************************************************************************
 *
 * Copyright (c) 2009-2010 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __HEXDUMP_H_A5111E31A8FD0B__
#define __HEXDUMP_H_A5111E31A8FD0B__

#include "debug.h"

namespace famouso {
    namespace mw {
        namespace afp {
            namespace shared {

                static uint8_t hexdump_get_hex_nibble(uint8_t c) {
                    return c + (c < 10 ? '0' : 'a' - 10);
                }

                /*!
                 * \brief Debugging helper that prints a hexdump to stderr.
                 * \param d Points to data to hexdump
                 * \param l Length of d in bytes.
                 */
                static void hexdump(const uint8_t * d, unsigned int l) {
                    const unsigned int per_line = 8;
                    unsigned int lines = l / per_line + (l % per_line == 0 ? 0 : 1);

                    for (unsigned int line = 0; line < lines; line++) {
                        ::logging::log::emit() << ::logging::log::tab;
                        for (unsigned int i = line * per_line; i < (line + 1) * per_line; i++) {
                            if (i >= l) {
                                for (int a = (line + 1) * per_line - i - 1; a >= 0; a--)
                                    ::logging::log::emit() << "   ";
                                break;
                            }
                            ::logging::log::emit() << " "
                                << hexdump_get_hex_nibble(d[i] >> 4)
                                << hexdump_get_hex_nibble(d[i] & 0xf);
                        }
                        ::logging::log::emit() << "   |   ";
                        for (unsigned int i = line * per_line; i < (line + 1) * per_line && i < l; i++) {
                            char show = (d[i] >= 32 && d[i] < 127 ? d[i] : '?');
                            ::logging::log::emit() << show;
                        }
                        ::logging::log::emit() << ::logging::log::endl;
                    }
                }


            } //namespace shared
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __HEXDUMP_H_A5111E31A8FD0B__

