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

#ifndef __MAXEVENTLENGTH_H_4517B510FC1BDA__
#define __MAXEVENTLENGTH_H_4517B510FC1BDA__

#include "mw/common/Event.h"

#include "mw/attributes/tags/IntegralConstTag.h"
#include "mw/attributes/Attribute.h"

#include "mw/attributes/detail/HighDensityIDs.h"

#include "mw/attributes/detail/tags/TagSet.h"
#include "mw/attributes/detail/tags/IsHighDensity.h"
#include "mw/attributes/detail/tags/HasLessThanRelation.h"

namespace famouso {
    namespace mw {
        namespace attributes {

            /*!
             * \brief Defines a configurable attribute for describing
             *        the maximum length events.
             *
             * The unit of the attribute value is bytes.
             *
             * \tparam size Describes the initial value to be set
             */
            template <famouso::mw::Event::Type size>
            class MaxEventLength : public Attribute<
                                            MaxEventLength<0>,
                                            tags::integral_const_tag,
                                            famouso::mw::Event::Type,
                                            size,
                                            detail::HighDensityIDs::maxEventLengthId,
                                            detail::TagSet<
                                                    detail::IsHighDensity
                                                   >
                                        > {
                public:
                    typedef MaxEventLength type;
            };

        } /* attributes */
    } /* mw */
} /* famouso */

#endif // __MAXEVENTLENGTH_H_4517B510FC1BDA__

