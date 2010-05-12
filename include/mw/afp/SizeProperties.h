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


#ifndef __SIZEPROPERTIES_H_5A17ADAD1EE863__
#define __SIZEPROPERTIES_H_5A17ADAD1EE863__


#include <stdint.h>
#include "config/type_traits/SmallestUnsignedTypeSelector.h"


namespace famouso {
    namespace mw {
        namespace afp {


            /*!
             *  \brief  SizeProperties config for default event size (max. 65535 bytes)
             */
            struct DefaultEventSizeProp {
                typedef uint16_t fcount_t;  ///< Fragment count type
                typedef uint16_t flen_t;    ///< Length type for fragment
                typedef uint16_t elen_t;    ///< Length type for fragmentation input (event)
            };


            /*!
             *  \brief  SizeProperties config for reduced event size (max. 255 bytes)
             */
            struct MinimalSizeProp {
                typedef uint8_t fcount_t;   ///< Fragment count type
                typedef uint8_t flen_t;     ///< Length type for fragment
                typedef uint8_t elen_t;     ///< Length type for fragmentation input (event)
            };


            /*!
             *  \brief  SizeProperties config for with automatic type selection
             *  \param  max_event_size  Maximum event size to support
             *  \param  mtu  Maximum transmission unit (default fragment size)
             */
            template <uint64_t max_event_size, uint64_t mtu>
            struct SizeProp {
                typedef typename SmallestUnsignedTypeSelector<max_event_size>::type elen_t;
                typedef typename SmallestUnsignedTypeSelector<mtu>::type flen_t;
                typedef typename SmallestUnsignedTypeSelector<max_event_size>::type fcount_t;
            };


        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __SIZEPROPERTIES_H_5A17ADAD1EE863__

