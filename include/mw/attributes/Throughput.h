/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __Bandwidth_h__
#define __Bandwidth_h__

#include <stdint.h>

#include "mw/attributes/tags/IntegralConstTag.h"
#include "mw/attributes/Attribute.h"

#include "mw/attributes/detail/HighDensityIDs.h"
#include "mw/attributes/filter/greater_than_or_equal_to.h"

namespace famouso {
    namespace mw {
        namespace attributes {

            /*!
             * \brief Defines a configurable throughput attribute for
             *  describing how many bytes per second can be transmitted.
             *
             * The unit of the attribute value is bytes per second.
             *
             * \tparam throughput Describes the initial value to be set
             */
            template<uint32_t throughput>
            class Throughput : public Attribute<
                                      Throughput<0>, tags::integral_const_tag,
                                      uint32_t, throughput, filter::greater_than_or_equal_to,
                                      detail::HighDensityIDs::throughput, true
                                     > {
                public:
                    typedef Throughput type;
            };

        } /* attributes */
    } /* mw */
} /* famouso */

#endif
