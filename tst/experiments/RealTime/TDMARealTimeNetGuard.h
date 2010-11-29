/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __TDMAREALTIMENETGUARD_H_1D30AC585BCF58__
#define __TDMAREALTIMENETGUARD_H_1D30AC585BCF58__

#include "debug.h"
#include "mw/common/Event.h"
#include "mw/nl/DistinctNL.h"
#include "mw/afp/Fragmenter.h"
#include "mw/afp/Defragmentation.h"

namespace famouso {
    namespace mw {
        namespace anl {

            // interface of an NL
            // Policy: different for polled and priority-based best efford traffic
            template <class NL>
            class RealTimeNetGuard : public NL {
                    typedef typename NL::packet_t packet_t;
                public:
                    // Guard against some timing faults
                    void deliver(const packet_t & p) {
                        // TODO: check, if we can deliver now
                        // in case of NRT (should be readable from p), this function
                        // may block and invoke the dispatcher to run another task
                        // TODO: return if success
                        NL::deliver(p);
                    }

                    // can hold own MEDL if (entkopplung) is wanted
            };

        } // namespace anl
    } // namespace mw
} //namespace famouso

#endif // __TDMAREALTIMENETGUARD_H_1D30AC585BCF58__

