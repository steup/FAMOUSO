/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *               2009-2010 Michael Kriese <kriese@cs.uni-magdeburg.de>
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

#ifndef __MAC_hpp__
#define __MAC_hpp__

#include <cstring>
#include "debug.h"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace _awds {

                /*! \brief A MAC address representing a client at awds.
                 */
                struct __attribute__((packed)) _MAC {

                    private:
                        typedef struct _MAC MAC;

                    public:

                        /*! \brief Parse a pointer and return a MAC address.
                         *
                         * \param data The pointer where the MAC can be located.
                         * \return A MAC address.
                         */
                        static MAC parse(void *data) {
                            MAC result;

                            std::memcpy(result._data, data, 6);

                            return result;
                        }

                        /*! \brief Copy the MAC address to a pointer.
                         *
                         * \param data A pointer where to copy the MAC.
                         */
                        void copy(void *data) {
                            std::memcpy(data, _data, 6);
                        }

                        /*! \brief Compare 2 MAC addresses of equality.
                         *
                         * \param o The other MAC address.
                         * \return true if the MAC addresses are equal, false otherwise.
                         */
                        bool operator==(const MAC &o) {
                            return std::memcmp(_data, o._data, 6) == 0;
                        }

                        friend ::logging::loggingReturnType &operator <<(::logging::loggingReturnType &out, const MAC &mac);

                    private:
                        uint8_t _data[6];
                };

                /*! \brief A MAC address representing a client at awds.
                 */
                typedef struct _MAC MAC;

                /*! \brief Print a MAC to log.
                 *
                 * \param out The log to print to.
                 * \param mac The MAC to print.
                 * \return The log to chain print calls.
                 */
                inline ::logging::loggingReturnType &operator <<(::logging::loggingReturnType &out, const MAC &mac) {
                    char buffer[30];
                    std::sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac._data[0], mac._data[1], mac._data[2], mac._data[3],
                            mac._data[4], mac._data[5]);
                    return out << buffer;
                }
            } /* _awds */

            namespace awds {

                /*! \brief A MAC address representing a client at awds.
                 */
                typedef famouso::mw::nl::_awds::MAC MAC;

            } /* awds */
        } /* nl */
    } /* mw */
} /* famouso */
#endif /* __MAC_hpp__ */
