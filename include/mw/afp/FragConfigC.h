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


#ifndef __FRAGCONFIGC_H_7C0434F7D77B7E__
#define __FRAGCONFIGC_H_7C0434F7D77B7E__


#include "boost/mpl/assert.hpp"


// Policies
#include "mw/afp/frag/Use32BitEventSeq.h"
#include "mw/afp/frag/UseNoEventSeq.h"

#include "mw/afp/frag/UseNoFEC.h"
#ifndef __AVR__
#include "mw/afp/frag/UseFEC.h"
#endif

#include "mw/afp/frag/RedundancyAttribute.h"

#include "object/Allocator.h"

#include "mw/afp/shared/OverflowErrorChecking.h"

#include "mw/afp/SizeProperties.h"


namespace famouso {
    namespace mw {
        namespace afp {


            struct UseNoEventSeq;
            struct UseEventSeq;
            struct UseNoFEC;

            template <class RedundancyAttribute>
            struct UseFEC {
                typedef RedundancyAttribute RedundancyAttrib;
            };

            /*
            // TODO: support multiple publishers per subject
            class OnePublisherPerSubject;
             */

            template <
                class EventSeqPolicy,
                class FECPolicy,
                /*
                 // TODO: support multiple publishers per subject
                 class PublisherDistinction,
                 */
                class SizeProperties = DefaultEventSizeProp,
                class Allocator = object::Allocator,
                class OverflowErrorCheckingPolicy = shared::OverflowErrorChecking
            >
            struct FragConfigC {
                // The combination of template parameters you chose is not a valid
                // configuration.
                BOOST_MPL_ASSERT_MSG(false, invalid_AFP_fragmentation_config, ());
            };

            template <
                class SizeProperties,
                class AllocatorType,
                class OverflowErrorCheckingPolicy
            >
            struct FragConfigC <
                        UseNoEventSeq,
                        UseNoFEC,
                        SizeProperties,
                        AllocatorType,
                        OverflowErrorCheckingPolicy
                        > {

                typedef SizeProperties SizeProp;

                typedef frag::UseNoEventSeq<FragConfigC> EventSeqUsagePolicy;
                typedef frag::UseNoFEC<FragConfigC> FECUsagePolicy;

                typedef AllocatorType Allocator;
                typedef OverflowErrorCheckingPolicy OverflowErrorChecking;
            };

            template <
                class SizeProperties,
                class AllocatorType,
                class OverflowErrorCheckingPolicy
            >
            struct FragConfigC <
                        UseEventSeq,
                        UseNoFEC,
                        SizeProperties,
                        AllocatorType,
                        OverflowErrorCheckingPolicy
                        > {

                typedef SizeProperties SizeProp;

                typedef frag::Use32BitEventSeq<FragConfigC> EventSeqUsagePolicy;
                typedef frag::UseNoFEC<FragConfigC> FECUsagePolicy;

                typedef AllocatorType Allocator;
                typedef OverflowErrorCheckingPolicy OverflowErrorChecking;
            };

#ifndef __AVR__
            template <
                class SizeProperties,
                class AllocatorType,
                class OverflowErrorCheckingPolicy,
                class RedAttrib
            >
            struct FragConfigC <
                        UseNoEventSeq,
                        UseFEC<RedAttrib>,
                        SizeProperties,
                        AllocatorType,
                        OverflowErrorCheckingPolicy
                        > {

                typedef SizeProperties SizeProp;

                typedef frag::UseNoEventSeq<FragConfigC> EventSeqUsagePolicy;
                typedef frag::UseFEC<FragConfigC> FECUsagePolicy;

                typedef typename UseFEC<RedAttrib>::RedundancyAttrib FECRedundancy;

                typedef AllocatorType Allocator;
                typedef OverflowErrorCheckingPolicy OverflowErrorChecking;
            };

            template <
                class SizeProperties,
                class AllocatorType,
                class OverflowErrorCheckingPolicy,
                class RedAttrib
            >
            struct FragConfigC <
                        UseEventSeq,
                        UseFEC<RedAttrib>,
                        SizeProperties,
                        AllocatorType,
                        OverflowErrorCheckingPolicy
                        > {

                typedef SizeProperties SizeProp;

                typedef frag::Use32BitEventSeq<FragConfigC> EventSeqUsagePolicy;
                typedef frag::UseFEC<FragConfigC> FECUsagePolicy;

                typedef typename UseFEC<RedAttrib>::RedundancyAttrib FECRedundancy;

                typedef AllocatorType Allocator;
                typedef OverflowErrorCheckingPolicy OverflowErrorChecking;
            };
#endif



        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __FRAGCONFIGC_H_7C0434F7D77B7E__

