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


#ifndef __COMMONPOLICYSELECTOR_H_1CE3B933BD15F0__
#define __COMMONPOLICYSELECTOR_H_1CE3B933BD15F0__


#include "config/type_traits/if_contains_select_type.h"
#include "config/type_traits/if_select_type.h"

#include "boost/mpl/assert.hpp"

#include "object/Allocator.h"
#include "mw/afp/shared/OverflowErrorChecking.h"
#include "mw/afp/SizeProperties.h"
#include "mw/afp/EmptyType.h"


namespace famouso {
    namespace mw {
        namespace afp {


            IF_CONTAINS_SELECT_TYPE_(Allocator);
            IF_CONTAINS_SELECT_TYPE_(SizeProperties);


            template <typename Config>
            struct CommonPolicySelector {
                /// Error checking policy
                typedef typename if_select_type<
                            Config::overflow_error_checking,
                            shared::OverflowErrorChecking,          // Full overflow checking
                            shared::NoOverflowErrorChecking         // Reduced error checking
                        >::type OverflowErrorChecking;

                /// Allocator to use
                typedef typename if_contains_select_type_Allocator<
                            Config,                                 // If Config contains type Allocator,
                                                                    // return it (user defined allocator)
                            object::Allocator                       // Else use default allocator
                        >::type Allocator;

                /// SizeProperties (data types used for fragment count, length and event length)
                typedef typename if_contains_select_type_SizeProperties<
                            Config,                                 // If Config contains type SizeProperties,
                                                                    // return it (user defined size properties)
                            DefaultEventSizeProp                    // Size types working for all valid events (default event type)
                        >::type SizeProp;

                BOOST_MPL_ASSERT_MSG(sizeof(typename SizeProp::flen_t) <= sizeof(typename SizeProp::elen_t),
                                     invalid_SizeProperties__event_length_type_smaller_than_fragment_length_type,
                                     (SizeProp));

                BOOST_MPL_ASSERT_MSG((Config::concurrent_events == 1) || Config::event_seq,
                                     invalid_config__event_seq_necessary_to_defrag_multiple_events_concurrently,
                                     ());
            };


        } // namespace afp
    } // namespace mw
} // namespace famouso

#endif // __COMMONPOLICYSELECTOR_H_1CE3B933BD15F0__

