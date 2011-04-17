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

#ifndef __REALTIMESLOTENDBOUNDARY_H_3D39C5C2E1E02D__
#define __REALTIMESLOTENDBOUNDARY_H_3D39C5C2E1E02D__

#include "mw/common/Event.h"

#include "mw/attributes/tags/IntegralConstTag.h"
#include "mw/attributes/Attribute.h"

#include "mw/attributes/detail/HighDensityIDs.h"

#include "mw/attributes/detail/tags/TagSet.h"
#include "mw/attributes/detail/tags/IsHighDensity.h"

namespace famouso {
    namespace mw {
        namespace attributes {

            /*!
             * \brief Defines a configurable attribute for describing
             *        the end boundary time of a real time
             *        communication channel slot.
             *
             * The unit of the attribute value is micro seconds.
             *
             * \tparam time Initial value to be set
             */
            template <uint64_t time>
            class RealTimeSlotEndBoundary : public Attribute<
                                            RealTimeSlotEndBoundary<0>,
                                            tags::integral_const_tag,
                                            uint64_t,
                                            time,
                                            detail::HighDensityIDs::rtSlotEndBoundId,
                                            detail::TagSet<
                                                    detail::IsHighDensity
                                                   >
                                        > {
                public:
                    typedef RealTimeSlotEndBoundary type;
            };

        } /* attributes */
    } /* mw */
} /* famouso */

#endif // __REALTIMESLOTENDBOUNDARY_H_3D39C5C2E1E02D__

