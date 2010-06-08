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

#ifndef __Absolute_Deadline_h__
#define __Absolute_Deadline_h__

#include <stdint.h>

#include "mw/attributes/tags/IntegralConstTag.h"
#include "mw/attributes/Attribute.h"

#include "mw/attributes/detail/SystemIDs.h"
#include "mw/attributes/filter/less_than_or_equal_to.h"

namespace famouso {
    namespace mw {
        namespace attributes {

            /*!
             * \brief Defines a configurable deadline attribute for
             *  describing a point in time when a specific event is
             *  expected to be occurred.
             *
             * The unit of the attribute value is the number of
             *  microseconds since 1970 0:00:00 GMT.
             *
             * \tparam deadline Describes the initial value to be set
             */
            template<uint64_t deadline>
            class AbsoluteDeadline : public Attribute<
                                       AbsoluteDeadline<0>, tags::integral_const_tag,
                                       uint64_t, deadline, filter::less_than_or_equal_to,
                                       detail::SystemIDs::absDeadline, true
                                      > {
                public:
                    typedef AbsoluteDeadline type;
            };

        } /* attributes */
    } /* mw */
} /* famouso */

#endif
