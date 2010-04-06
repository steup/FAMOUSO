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


#ifndef __DEFRAGCONFIGC_H_113525CAC772E0__
#define __DEFRAGCONFIGC_H_113525CAC772E0__


#include "boost/mpl/assert.hpp"


// Policies
#ifndef __AVR__
#include "mw/afp/defrag/EventSeqDemux.h"
#include "mw/afp/defrag/MultiSourceDemux.h"
#endif
#include "mw/afp/defrag/SingleEventDemux.h"

#include "mw/afp/defrag/NoFECHeaderSupport.h"
#ifndef __AVR__
#include "mw/afp/defrag/FECHeaderSupport.h"
#endif

#include "mw/afp/defrag/InOrderEventDataReconstructor.h"
#include "mw/afp/defrag/OutOfOrderEventDataReconstructor.h"
#ifndef __AVR__
#include "mw/afp/defrag/FECEventDataReconstructor.h"
#endif

#include "mw/afp/defrag/NoDuplicateChecker.h"
#ifndef __AVR__
#include "mw/afp/defrag/DuplicateChecker.h"
#endif

#include "object/Allocator.h"
#include "mw/afp/defrag/Statistics.h"

#include "mw/afp/shared/OverflowErrorChecking.h"

#include "mw/afp/SizeProperties.h"

#include "mw/afp/EmptyType.h"


namespace famouso {
    namespace mw {
        namespace afp {


            struct OneSubject;

            template <class Subject>
            struct MultipleSubjects {
                typedef Subject SubjectType;
            };


            class EventSeqSupport;
            class NoEventSeqSupport;

            class DuplicateChecking;
            class NoDuplicateChecking;

            class ReorderingSupport;
            class NoReorderingSupport;

            class NoFECSupport;
            class FECSupport;


            template <
                class DefragConfigC,
                class SubjectNumber,
                class EventSeqPolicy
            >
            struct DefragConfigCDemuxConfig {
                // The combination of template parameters you chose is not a valid
                // configuration.
                BOOST_MPL_ASSERT_MSG(false, invalid_AFP_defragmentation_config, ());
            };

            template <class DefragConfigC>
            struct DefragConfigCDemuxConfig < DefragConfigC, OneSubject, NoEventSeqSupport > {
                typedef EmptyType SubjectType;
                typedef defrag::EmptyDemuxKey<DefragConfigC, SubjectType> EventDemuxKey;
                typedef defrag::SingleEventDemux<DefragConfigC> DemuxPolicy;
            };

            template <class DefragConfigC>
            struct DefragConfigCDemuxConfig < DefragConfigC, OneSubject, EventSeqSupport > {
                typedef EmptyType SubjectType;
                typedef defrag::EseqDemuxKey<DefragConfigC, SubjectType> EventDemuxKey;
                typedef defrag::EventSeqDemux<DefragConfigC> DemuxPolicy;
            };

            template <class DefragConfigC, class SubjectT>
            struct DefragConfigCDemuxConfig < DefragConfigC, MultipleSubjects<SubjectT>, NoEventSeqSupport > {
                typedef SubjectT SubjectType;
                typedef defrag::SubjectDemuxKey<DefragConfigC, SubjectType> EventDemuxKey;
                typedef defrag::MultiSourceDemux<DefragConfigC> DemuxPolicy;
            };

            template <class DefragConfigC, class SubjectT>
            struct DefragConfigCDemuxConfig < DefragConfigC, MultipleSubjects<SubjectT>, EventSeqSupport > {
                typedef SubjectT SubjectType;
                typedef defrag::EseqSubjectDemuxKey<DefragConfigC, SubjectType> EventDemuxKey;
                typedef defrag::EventSeqDemux<DefragConfigC> DemuxPolicy;
            };



            template < class DefragConfigC, class DuplicatePolicy >
            struct DefragConfigCDuplicateConfig {
                // The combination of template parameters you chose is not a valid
                // configuration.
                BOOST_MPL_ASSERT_MSG(false, invalid_AFP_defragmentation_config, ());
            };

            template < class DefragConfigC >
            struct DefragConfigCDuplicateConfig <DefragConfigC, NoDuplicateChecking> {
                typedef defrag::NoDuplicateChecker<DefragConfigC> DuplicateCheckingPolicy;
            };

            template < class DefragConfigC >
            struct DefragConfigCDuplicateConfig <DefragConfigC, DuplicateChecking> {
                typedef defrag::DuplicateChecker<DefragConfigC> DuplicateCheckingPolicy;
            };



            template < class DefragConfigC, class ReorderingPolicy, class FECPolicy >
            struct DefragConfigCEventDataReconstructionConfig {
                // The combination of template parameters you chose is not a valid
                // configuration.
                BOOST_MPL_ASSERT_MSG(false, invalid_AFP_defragmentation_config, ());
            };

            template < class DefragConfigC >
            struct DefragConfigCEventDataReconstructionConfig <
                        DefragConfigC,
                        NoReorderingSupport,
                        NoFECSupport
                        > {
                typedef defrag::InOrderEventDataReconstructor<DefragConfigC> EventDataReconstructionPolicy;
            };

            template < class DefragConfigC >
            struct DefragConfigCEventDataReconstructionConfig <
                        DefragConfigC,
                        ReorderingSupport,
                        NoFECSupport
                        > {
                typedef defrag::OutOfOrderEventDataReconstructor<DefragConfigC> EventDataReconstructionPolicy;
            };

#ifndef __AVR__
            template < class DefragConfigC, class ReorderingPolicy >
            struct DefragConfigCEventDataReconstructionConfig <
                        DefragConfigC,
                        ReorderingPolicy,
                        FECSupport
                        > {
                typedef defrag::FECEventDataReconstructor<DefragConfigC> EventDataReconstructionPolicy;
            };
#endif


            template <
                /*
                 // TODO: support multiple publishers per subject
                 class PublisherDistinction,
                 */
                class SubjectNumber,
                class EventSeqPolicy,
                class DuplicatePolicy,
                class ReorderingPolicy,
                class FECPolicy,
                class SizeProperties = DefaultEventSizeProp,
                class AllocatorType = object::Allocator,
                class StatisticsPolicy = defrag::NoStatistics,
                class OverflowErrorCheckingPolicy = shared::OverflowErrorChecking
            >
            struct DefragConfigC :
                DefragConfigCDemuxConfig <
                    DefragConfigC<SubjectNumber, EventSeqPolicy, DuplicatePolicy, ReorderingPolicy, FECPolicy, SizeProperties, AllocatorType, StatisticsPolicy, OverflowErrorCheckingPolicy>,
                    SubjectNumber,
                    EventSeqPolicy
                >,
                DefragConfigCDuplicateConfig <
                    DefragConfigC< SubjectNumber, EventSeqPolicy, DuplicatePolicy, ReorderingPolicy, FECPolicy, SizeProperties, AllocatorType, StatisticsPolicy, OverflowErrorCheckingPolicy>,
                    DuplicatePolicy
                >,
                DefragConfigCEventDataReconstructionConfig <
                    DefragConfigC< SubjectNumber, EventSeqPolicy, DuplicatePolicy, ReorderingPolicy, FECPolicy, SizeProperties, AllocatorType, StatisticsPolicy, OverflowErrorCheckingPolicy>,
                    ReorderingPolicy,
                    FECPolicy
                >
            {
                typedef SizeProperties SizeProp;

                typedef AllocatorType Allocator;
                typedef StatisticsPolicy DefragStatistics;
                typedef OverflowErrorCheckingPolicy OverflowErrorChecking;
            };


        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __DEFRAGCONFIGC_H_113525CAC772E0__

