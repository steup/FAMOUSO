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


#ifndef __FRAGPOLICYSELECTOR_H_7C0434F7D77B7E__
#define __FRAGPOLICYSELECTOR_H_7C0434F7D77B7E__


#include "mw/afp/CommonPolicySelector.h"


// Policies
#include "mw/afp/frag/NoFECFragmenter.h"
#include "mw/afp/frag/FECFragmenter.h"

#include "mw/afp/frag/Use32BitEventSeq.h"
#include "mw/afp/frag/UseNoEventSeq.h"

#include "mw/afp/frag/RedundancyAttribute.h"

#include "mw/afp/EmptyType.h"



namespace famouso {
    namespace mw {
        namespace afp {

            /*!
             *  \brief  Default redundancy attribute
             *
             *  Use \p NonDefaultPolicies parameter of FragType to use a different
             *  redundancy attribute, e.g. to use different redundancies for multiple
             *  channels.
             */
            typedef frag::RedundancyAttribute<EmptyType, 0> FragmentationRedundancy;

            IF_CONTAINS_SELECT_TYPE_(RedundancyAttribute);

            // TODO: support multiple publishers per subject
            template <typename Config>
            struct FragPolicySelector {
                typedef FragPolicySelector ThisType;
                typedef CommonPolicySelector<Config> Common;

                /// Event sequence numbering policy
                typedef typename if_select_type<
                            Config::event_seq,
                            frag::Use32BitEventSeq<ThisType>,
                            frag::UseNoEventSeq<ThisType>
                        >::type EventSeqUsagePolicy;

                /// Fragmentation/FEC usage policy
                typedef typename if_select_type<
                            Config::FEC,
                            frag::FECFragmenter<ThisType>,
                            frag::NoFECFragmenter<ThisType>
                        >::type FragImplPolicy;

                /// FEC redundancy attribute (access to redundancy at run-time)
                typedef typename if_select_type<
                            Config::FEC,
                            typename if_contains_select_type_RedundancyAttribute<
                                Config,                     // If Config contains RedundancyAttribute
                                                            // return it
                                FragmentationRedundancy     // Otherwise return default redundancy attribute
                            >::type,
                            EmptyType                       // No FEC. FECRedundancy unused...
                        >::type FECRedundancy;

                /// Overflow error checking policy
                typedef typename Common::OverflowErrorChecking OverflowErrorChecking;

                /// Allocator to use
                typedef typename Common::Allocator Allocator;

                /// SizeProperties
                typedef typename Common::SizeProp SizeProp;
            };

        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __FRAGPOLICYSELECTOR_H_7C0434F7D77B7E__

