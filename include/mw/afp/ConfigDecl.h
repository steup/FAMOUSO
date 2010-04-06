/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
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


#ifndef __CONFIGDECL_H_6D03524A376067__
#define __CONFIGDECL_H_6D03524A376067__


#include <stdint.h>


namespace famouso {
    namespace mw {
        namespace afp {

            enum AFPProperties {
                ideal_network = 0,
                packet_loss = 1,
                reordering = 2,
                duplicates = 4,

                no_event_overlap = 8,
                event_overlap = 0,

                one_subject = 16,
                multiple_subjects = 0,

                FEC = 32,                ///< nur sinnvoll bei packet_loss
                NoFEC = 0,

                defrag_statistics = 64,
                no_defrag_statistics = 0,

                ///
                no_overflow_error_checking = 128,
                overflow_error_checking = 0,

                /// nur, wenn in NonDefaultPolicies kein SizeProperties
                max_event_length_255 = 256,

            };

            typedef uint32_t ConfigFlagsType;

        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __CONFIGDECL_H_6D03524A376067__

