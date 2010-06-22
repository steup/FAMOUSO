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


#ifndef __DEFRAGMENTATION_H_AA773DC94A45F8__
#define __DEFRAGMENTATION_H_AA773DC94A45F8__


#include "boost/mpl/assert.hpp"

#include "debug.h"

#include "mw/afp/Config.h"
#include "mw/afp/DefragPolicySelector.h"
#include "mw/afp/defrag/Defragmenter.h"
#include "mw/afp/defrag/Headers.h"
#include "mw/afp/EmptyType.h"


namespace famouso {
    namespace mw {
        namespace afp {


            template <class AFPDC> class DefragmentationStep;


            /*!
             * \brief Defragmentation engine that processes defragmentation steps
             *
             * Contains pool of events whose reconstruction is in progress.
             * Create one permanent instance per channel or network.
             *
             * \tparam AFPDC AFP defragmentation config
             * \see DefragmentationStep
             */
            template <class AFPDC>
            class DefragmentationProcessor {

                protected:

                    typedef DefragPolicySelector<AFPDC> DCP;

                    typedef typename DCP::SizeProp::elen_t elen_t;
                    typedef typename DCP::SizeProp::flen_t flen_t;
                    typedef typename DCP::SizeProp::fcount_t fcount_t;

                    typedef class DCP::DemuxPolicy DemuxPolicy;
                    typedef class DCP::DefragStatistics Statistics;


                    /// Event demultiplexer
                    DemuxPolicy demux;

                public:

                    /*!
                     * \brief Constrcutor
                     * \param mtu Maximum transmission unit (max. size of fragment inclusive AFP headers)
                     */
                    DefragmentationProcessor(flen_t mtu)
                            : demux(mtu) {
                    }


                    /*!
                     * \brief Processes fragment.
                     * \param ds Contains fragment data.
                     *
                     * After returning an event may be complete (check ds.event_complete()).
                     */
                    void process_fragment(DefragmentationStep<AFPDC> & ds) {
                        Statistics::fragment_received();

                        if (ds.fragment_header.error()) {
                            Statistics::fragment_incorrect_header();
                            return;
                        }

                        ds.defragmenter_handle = demux.get_defragmenter_handle(ds.fragment_header, ds.event_demux_key);

                        if (ds.defragmenter_handle) {
                            defrag::Defragmenter<DCP> * defrag = demux.get_defragmenter(ds.defragmenter_handle);

                            defrag->put_fragment(ds.fragment_header, ds.fragment_payload, ds.fragment_payload_length);

                            if (defrag->error()) {
                                // Free event data if this event cannot be reconstructed for sure.
                                demux.free_defragmenter(ds.defragmenter_handle);
                            } else {
                                // Check event reconstruction status and return data if event is complete
                                if (defrag->is_event_complete()) {
                                    ds.event_data = defrag->get_event_data();
                                    ds.event_length = defrag->get_event_length();
                                }
                            }
                        } else {
                            // This fragment is outdated. The event it belongs to
                            // was already reconstructed (this fragment is a late duplicate
                            // or redundancy we do not need) or already dropped because of
                            // timeouts.
                            Statistics::fragment_outdated();
                        }
                    }

                    /*!
                     * \brief Free event. Call this after data of an complete event was processed.
                     * \param ds Contains event data.
                     */
                    void event_processed(const DefragmentationStep<AFPDC> & ds) {
                        FAMOUSO_ASSERT(ds.event_complete());
                        if (ds.defragmenter_handle)
                            demux.free_defragmenter(ds.defragmenter_handle);
                    }
            };


            /*!
             * \brief Defragmentation processor that supports keeping events available for later delivery.
             *
             * \tparam AFPDC AFP defragmentation config
             */
            template <class AFPDC>
            class DefragmentationProcessorKeepEventSupport : public DefragmentationProcessor<AFPDC> {

                    typedef DefragPolicySelector<AFPDC> DCP;

                    typedef typename DCP::SizeProp::flen_t flen_t;
                    typedef typename DCP::SizeProp::elen_t elen_t;

                public:

                    /// Ensure that demultiplexing policy supports late delivery
                    enum { AFP_feature_check = DCP::DemuxPolicy::support_late_delivery };

                    /*!
                     * \brief Constrcutor
                     * \param mtu Maximum transmission unit (max. size of fragment inclusive AFP headers)
                     */
                    DefragmentationProcessorKeepEventSupport(flen_t mtu)
                            : DefragmentationProcessor<AFPDC> (mtu) {
                    }

                    /*!
                     * \brief Keep completed event for later delivery.
                     * \return Event handle. 0 on error.
                     */
                    void * keep_event(const DefragmentationStep<AFPDC> & ds) {
                        // Check whether this fragment was assigned to an complete event
                        if (!ds.defragmenter_handle || ds.event_data == 0)
                            return 0;

                        return DefragmentationProcessor<AFPDC>::demux.keep_defragmenter(ds.defragmenter_handle);
                    }

                    /*!
                     * \brief Get data of kept event.
                     * \param event_handle Handle returned by keep_event().
                     * \return Data pointer
                     */
                    uint8_t * kept_get_event_data(void * event_handle) {
                        defrag::Defragmenter<DCP> * defrag = DefragmentationProcessor<AFPDC>::demux.get_kept_defragmenter(event_handle);
                        return defrag->get_event_data();
                    }

                    /*!
                     * \brief Get length of kept event.
                     * \param event_handle Handle returned by keep_event().
                     * \return Data length
                     */
                    elen_t kept_get_event_length(void * event_handle) {
                        defrag::Defragmenter<DCP> * defrag = DefragmentationProcessor<AFPDC>::demux.get_kept_defragmenter(event_handle);
                        return defrag->get_event_length();
                    }

                    /*!
                     * \brief Free kept event.
                     * \param event_handle Handle returned by keep_event().
                     */
                    void kept_event_processed(void * event_handle) {
                        DefragmentationProcessor<AFPDC>::demux.free_kept_defragmenter(event_handle);
                    }
            };


            /*!
             * \brief Defragmentation processor adapted to the needs of the ANL
             *
             * Provides single-thread-only interface not needing the DefragmentationStep
             * for freeing an event.
             *
             * \tparam AFPDC AFP defragmentation config
             * \see DefragmentationStep
             */
            template <class AFPDC>
            class DefragmentationProcessorANL : private DefragmentationProcessor<AFPDC> {

                    typedef DefragmentationProcessor<AFPDC> Base;
                    typedef typename Base::flen_t flen_t;

                protected:

                    /// Defragmenter handle of last complete event.
                    /// Zero if there is none or it was already freed.
                    void * last_defragmenter_handle;

                public:

                    /*!
                     * \brief Constrcutor
                     * \param mtu Maximum transmission unit (max. size of fragment inclusive AFP headers)
                     */
                    DefragmentationProcessorANL(flen_t mtu)
                            : Base(mtu), last_defragmenter_handle(0) {
                    }


                    /*!
                     * \brief Processes fragment.
                     * \param ds Contains fragment data.
                     *
                     * After returning an event may be complete (check ds.event_complete()).
                     */
                    void process_fragment(DefragmentationStep<AFPDC> & ds) {
                        Base::process_fragment(ds);
                        if (ds.event_complete())
                            last_defragmenter_handle = ds.defragmenter_handle;
                    }

                    /*!
                     * \brief Free last completed event. Call this after data of an complete event was processed.
                     *
                     * Checks whether there is something to free. Thus, it can be always called after processing
                     */
                    void last_event_processed() {
                        if (last_defragmenter_handle) {
                            Base::demux.free_defragmenter(last_defragmenter_handle);
                            last_defragmenter_handle = 0;
                        }
                    }
            };

            /*!
             *  \brief  Empty version of DefragmentationProcessorANL for disabling defragmentation in the ANL
             */
            template <>
            class DefragmentationProcessorANL<Disable>  {

                    typedef uint64_t flen_t;

                public:

                    /*!
                     * \brief Constrcutor
                     * \param mtu Maximum transmission unit (max. size of fragment inclusive AFP headers)
                     */
                    DefragmentationProcessorANL(flen_t mtu) {
                    }


                    /*!
                     * \brief Processes fragment.
                     * \param ds Contains fragment data.
                     *
                     * After returning an event may be complete (check ds.event_complete()).
                     */
                    void process_fragment(DefragmentationStep<Disable> & ds) {
                    }

                    /*!
                     * \brief Free last complete event. Call this after data of an complete event was processed.
                     *
                     * Checks whether there is something to free. Thus, it can be always called after processing
                     */
                    void last_event_processed() {
                    }
            };


            /*!
             * \brief Contains temporary data needed for a single defragmentation step
             *        (reading fragments header, processing fragment and if
             *        event is complete returning buffer, process event, free event)
             *
             * Create a new instance (on the stack) for every fragment you receive.
             *
             * \tparam AFPDC AFP defragmentation config
             */
            template <class AFPDC>
            class DefragmentationStep {

                    typedef DefragPolicySelector<AFPDC> DCP;

                    typedef typename DCP::SizeProp::elen_t elen_t;
                    typedef typename DCP::SizeProp::flen_t flen_t;
                    typedef typename DCP::SizeProp::fcount_t fcount_t;

                    typedef typename DCP::SubjectType SubjectType;
                    typedef typename DCP::EventDemuxKey DemuxKeyType;

                protected:

                    /// Header of the fragment
                    defrag::Headers<DCP> fragment_header;

                    /// Pointer to fragment's payload
                    const uint8_t * fragment_payload;

                    /// Length of fragment's payload
                    flen_t fragment_payload_length;

                    /// Key to identify the event this fragment belongs to
                    DemuxKeyType event_demux_key;

                    /// Handle of defragmenter this fragment belongs to
                    void * defragmenter_handle;

                    /// Pointer to event's data. Null if event is not complete yet.
                    uint8_t * event_data;

                    /// Event's data length
                    elen_t event_length;

                    friend class DefragmentationProcessor<AFPDC>;
                    friend class DefragmentationProcessorKeepEventSupport<AFPDC>;
                    friend class DefragmentationProcessorANL<AFPDC>;

                public:

                    /*!
                     * \brief Construct object and read header
                     *
                     * Use this constructor for multiple subject configurations.
                     * If you use this constructor for one subject configurations,
                     * the subject parameter will be ignored.
                     */
                    DefragmentationStep(const uint8_t * fdata, flen_t flength, const SubjectType & subject /* TODO: Absender-Knoten */) :
                            fragment_header(fdata),
                            fragment_payload(fdata + fragment_header.length()),
                            fragment_payload_length(flength - fragment_header.length()),
                            event_demux_key(fragment_header, subject),
                            defragmenter_handle(0),
                            event_data(0),
                            event_length(0) {
                    }

                    /*!
                     * \brief Construct object and read header
                     *
                     * Use this constructor for one subject configurations
                     */
                    DefragmentationStep(const uint8_t * fdata, flen_t flength /* TODO: Absender-Knoten */) :
                            fragment_header(fdata),
                            fragment_payload(fdata + fragment_header.length()),
                            fragment_payload_length(flength - fragment_header.length()),
                            event_demux_key(fragment_header, EmptyType()),
                            defragmenter_handle(0),
                            event_data(0),
                            event_length(0) {
                        BOOST_MPL_ASSERT_MSG(!DemuxKeyType::uses_subject, wrong_constructor_for_multiple_subject_configurations, ());
                    }

                    /*!
                     * \brief Return whether an error occured while reading header.
                     *
                     * DefragmentationProcessor::process() checks this as well.
                     * You needn't call this if you are not interested whether an error
                     * occured or not.
                     */
                    bool error() const {
                        return fragment_header.error();
                    }

                    /// Returns whether an event is complete after fragment has been processed.
                    bool event_complete() const {
                        return event_data != 0;
                    }

                    /// Returns pointer to event data or NULL if event is not complete yet.
                    uint8_t * get_event_data() const {
                        return event_data;
                    }

                    /// Returns length of the event.
                    elen_t get_event_length() const {
                        return event_length;
                    }
            };


            /*!
             *  \brief  Empty version of DefragmentationStep for disabling defragmentation in the ANL
             */
            template <>
            class DefragmentationStep<Disable> {

                    typedef uint64_t elen_t;
                    typedef uint64_t flen_t;

                public:

                    /*!
                     * \brief Construct object and read header
                     *
                     * Use this constructor for multiple subject configurations.
                     * If you use this constructor for one subject configurations,
                     * the subject parameter will be ignored.
                     */
                    template <typename SubjectType>
                    DefragmentationStep(const uint8_t * fdata, flen_t flength, const SubjectType & subject /* TODO: Absender-Knoten */) {
                        ::logging::log::emit< ::logging::Warning>()
                            << PROGMEMSTRING("Dropping fragment. Defragmentation is disabled.")
                            << ::logging::log::endl;
                    }

                    /*!
                     * \brief Construct object and read header
                     *
                     * Use this constructor for one subject configurations
                     */
                    DefragmentationStep(const uint8_t * fdata, flen_t flength /* TODO: Absender-Knoten */) {
                        ::logging::log::emit< ::logging::Warning>()
                            << PROGMEMSTRING("Dropping fragment. Defragmentation is disabled.")
                            << ::logging::log::endl;
                    }

                    /*!
                     * \brief Return whether an error occured while reading header.
                     *
                     * DefragmentationProcessor::process() checks this as well.
                     * You needn't call this if you are not interested whether an error
                     * occured or not.
                     */
                    bool error() const {
                        return true;
                    }

                    /// Returns whether an event is complete after fragment has been processed.
                    bool event_complete() const {
                        return false;
                    }

                    /// Returns pointer to event data or NULL if event is not complete yet.
                    uint8_t * get_event_data() const {
                        return 0;
                    }

                    /// Returns length of the event.
                    elen_t get_event_length() const {
                        return 0;
                    }
            };


        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __DEFRAGMENTATION_H_AA773DC94A45F8__

