/*******************************************************************************
 *
 * Copyright (c) 2009-2010 Philipp Werner <philipp.werner@st.ovgu.de>
 *                         Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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


#ifndef __LARGEEVENT_H_835EBCE7273CAA__
#define __LARGEEVENT_H_835EBCE7273CAA__


#include "mw/common/Subject.h"


namespace famouso {
    namespace mw {
        namespace afp {

            using famouso::mw::Subject;

            /*!
             * \brief Event that may contain more than 65535 bytes data (up to 4GB).
             */
            struct LargeEvent {
                typedef uint32_t Type;
                const Subject &subject;
                Type length;
                uint8_t *data;

                LargeEvent(const Subject &s): subject(s), length(0), data(0) {}

                uint8_t & operator[](Type i) {
                    return data[i];
                }

                uint8_t & operator[](Type i) const {
                    return data[i];
                }
            };


            /*!
             *  \brief  SizeProperties config for large events (up to 4GB). Application layer only.
             *  \see    LargeEvent
             */
            struct LargeEventSizeProp {
                typedef uint32_t fcount_t;   ///< Fragment count type
                typedef uint16_t flen_t;     ///< Length type for fragment
                typedef uint32_t elen_t;     ///< Length type for fragmentation input (event)
            };


        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __LARGEEVENT_H_835EBCE7273CAA__

