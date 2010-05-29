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

#ifndef __NetworkID_hpp__
#define __NetworkID_hpp__

#include <cstring>
#include "debug.h"

namespace famouso {
    namespace mw {
        namespace nl {

            /*! \brief An ID representing a node at the network.
             *
             * \tparam size The number of bytes the id can be long.
             */
            template< uint16_t size = 0 >
            struct NetworkID {

                    typedef NetworkID<size> type;

                    /*! \brief Parse a pointer and return a NetworkID.
                     *
                     * \param data The pointer where the NetworkID can be located.
                     * \return A NetworkID.
                     */
                    static NetworkID parse(uint8_t data[size]) {
                        NetworkID result;

                        std::memcpy(result._data, data, size);

                        return result;
                    }

                    /*! \brief Compare 2 NetworkIDs of equality.
                     *
                     * \param o The other NetworkID.
                     * \return true if the NetworkIDs are equal, false otherwise.
                     */
                    bool operator==(const NetworkID &o) const {
                        return std::memcmp(_data, o._data, size) == 0;
                    }

                    /** \brief Compare two network ids for sorting.
                     *  \param o The other network id to compare with.
                     *  \return True if this id is smaller than the other id, otherwise false.
                     */
                    bool operator<(const NetworkID &o) const {
                        return std::memcmp(_data, o._data, size) < 0;
                    }

                    /*! \brief prints the NetworkID to the stream.
                     *
                     *  \param out The output stream to print to.
                     */
                    void print(::logging::loggingReturnType &out) const {
                        char tmp;
                        for (uint16_t i = 0; i < size; i++) {
                            if (i > 0)
                                out << ":";
                            // upper 4 BIT
                            tmp = _data[i] >> 4;
                            out << (char) (tmp < 10 ? '0' + tmp : 'A' + tmp - 10);

                            // lower 4 BIT
                            tmp = _data[i] & 0xF;
                            out << (char) (tmp < 10 ? '0' + tmp : 'A' + tmp - 10);
                        }
                    }

                private:
                    uint8_t _data[size];
            }__attribute__((packed));
        } /* nl */
    } /* mw */
} /* famouso */

namespace logging {
    /*! \brief Print a Network ID to a log.
     *
     * \param out The log to print to.
     * \param id The Network ID to print.
     * \return The log to chain print calls.
     */
    template< uint16_t size >
    inline ::logging::loggingReturnType &operator <<(::logging::loggingReturnType &out, const famouso::mw::nl::NetworkID<size> &id) {
        id.print(out);
        return out;
    }
} // namespace logging
#endif /* __NetworkID_hpp__ */
