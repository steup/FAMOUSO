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


#ifndef __DEFRAGPOLICYSELECTOR_H_113525CAC772E0__
#define __DEFRAGPOLICYSELECTOR_H_113525CAC772E0__


#include "mw/afp/CommonPolicySelector.h"
#include "mw/afp/EmptyType.h"


// Policies
#include "mw/afp/defrag/EventSeqHeaderSupport.h"
#include "mw/afp/defrag/NoEventSeqHeaderSupport.h"

#include "mw/afp/defrag/NoFECHeaderSupport.h"
#include "mw/afp/defrag/FECHeaderSupport.h"

#include "mw/afp/defrag/EventSeqDemux.h"
#include "mw/afp/defrag/SingleEventDemux.h"
#include "mw/afp/defrag/MultiSourceDemux.h"

#include "mw/afp/defrag/InOrderEventDataReconstructor.h"
#include "mw/afp/defrag/OutOfOrderEventDataReconstructor.h"
#include "mw/afp/defrag/FECEventDataReconstructor.h"

#include "mw/afp/defrag/NoDuplicateChecker.h"
#include "mw/afp/defrag/DuplicateChecker.h"

#include "mw/afp/defrag/Statistics.h"


namespace famouso {
    namespace mw {
        namespace afp {

            /*!
             *  \brief  Default statistics collector
             *
             *  Define a type DefragStatistics in the AFP config to use a different
             *  statistics collector, e.g. to collect statistics seperatly for multiple
             *  channels.
             */
            typedef defrag::Statistics<EmptyType> DefragmentationStatistics;

            IF_CONTAINS_SELECT_TYPE_(SubjectType);
            IF_CONTAINS_SELECT_TYPE_(DefragStatistics);

            // TODO: support multiple publishers per subject

            // config: not only underlying net, but what to support
            template <typename Config>
            struct DefragPolicySelector {
                typedef DefragPolicySelector ThisType;
                typedef CommonPolicySelector<Config> Common;

                /// Demultiplexing policy
                typedef typename if_select_type<
                            !Config::event_seq,
                            typename if_select_type<
                                        Config::multiple_subjects,
                                        defrag::MultiSourceDemux<ThisType>,   // no eseq and multiple subjects
                                        defrag::SingleEventDemux<ThisType>    // no eseq and one subject
                            >::type,
                            defrag::EventSeqDemux<ThisType>                   // eseq and maybe multiple subjects
                        >::type DemuxPolicy;

                /// Duplicate checking policy
                typedef typename if_select_type<
                            Config::duplicates,
                            defrag::DuplicateChecker<ThisType>,               // dupliactes
                            defrag::NoDuplicateChecker<ThisType>              // no duplicates
                        >::type DuplicateCheckingPolicy;

                /// Event data reconstruction policy
                typedef typename if_select_type<
                            Config::FEC,
                            defrag::FECEventDataReconstructor<ThisType>,                    // FEC support
                            typename if_select_type<
                                        Config::reordering,
                                        defrag::OutOfOrderEventDataReconstructor<ThisType>, // reordering
                                        defrag::InOrderEventDataReconstructor<ThisType>     // no reordering
                            >::type
                        >::type EventDataReconstructionPolicy;

                /// Internal subject type
                typedef typename if_contains_select_type_SubjectType<
                            Config,
                            EmptyType
                        >::type SubjectType;

                /// Event demultiplexing key type (used to distinguish events)
                typedef typename if_select_type<
                            !Config::event_seq,
                            typename if_select_type<
                                        Config::multiple_subjects,
                                        defrag::SubjectDemuxKey<ThisType, SubjectType>, // ideal net and multiple subjects
                                        defrag::EmptyDemuxKey<ThisType, SubjectType>    // ideal net and one subject
                            >::type,
                            typename if_select_type<
                                        Config::multiple_subjects,
                                        defrag::EseqSubjectDemuxKey<ThisType, SubjectType>, // non ideal net and multiple subjects
                                        defrag::EseqDemuxKey<ThisType, SubjectType>         // non ideal net and one subject
                            >::type
                        >::type EventDemuxKey;


                /// Defragmentation statistics policy
                typedef typename if_select_type<
                            Config::defrag_statistics,
                            typename if_contains_select_type_DefragStatistics<
                                Config,                         // Return Config::DefragStatistics if defined
                                DefragmentationStatistics       // Default statistics otherwise
                            >::type,
                            defrag::NoStatistics                // No statistics
                        >::type DefragStatistics;

                /// Overflow error checking policy
                typedef typename Common::OverflowErrorChecking OverflowErrorChecking;

                /// Allocator to use
                typedef typename Common::Allocator Allocator;

                /// SizeProperties
                typedef typename Common::SizeProp SizeProp;

                enum {
                    /// Used by EventSeqDemux and MultiSourceDemux (max number of Defragmenters)
                    concurrent_events = Config::concurrent_events,

                    /// Used by EventSeqDemux (max elements of late duplicate lookup)
                    old_event_ids = Config::old_event_ids,

                    /// Used by DuplicateChecker
                    max_fragments = Config::max_fragments
                };
            };

        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __DEFRAGPOLICYSELECTOR_H_113525CAC772E0__

