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


#ifndef __EVENTSEQDEMUX_H_2A8B3BD61E7528__
#define __EVENTSEQDEMUX_H_2A8B3BD61E7528__


#include "debug.h"

#include "mw/afp/defrag/Defragmenter.h"

#include "mw/afp/shared/Time.h"
#include "mw/afp/defrag/detail/PointerMap.h"
#include "mw/afp/defrag/detail/Queue.h"

#include "mw/afp/defrag/EventSeqHeaderSupport.h"



namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {


                /*!
                 * \brief Demux key type with event sequence number, one subject and one publisher
                 */
                template <typename DCP, class Subject_t>
                struct EseqDemuxKey {
                    const uint32_t eseq;
                    const bool eseq_header;      /// True if header did contain an event sequence number

                    enum { uses_subject = 0 };

                    bool operator<(const EseqDemuxKey & v2) const {
                        return eseq < v2.eseq;
                    }

                    bool operator==(const EseqDemuxKey & v2) const {
                        return eseq == v2.eseq;
                    }

                    EseqDemuxKey(const Headers<DCP> & header, const Subject_t & subj) :
                            eseq(header.eseq.occurs() ?
                                 header.eseq.get_eseq() :
                                 0xffffffff),
                            eseq_header(header.eseq.occurs()) {
                    }
                };


                /*!
                 * \brief Demux key type with event sequence number, multiple subjects and one publisher per subject
                 */
                template <typename DCP, class Subject_t>
                struct EseqSubjectDemuxKey {
                    const uint32_t eseq;
                    const Subject_t subject;
                    const bool eseq_header;      /// True if header did contain an event sequence number

                    enum { uses_subject = 1 };

                    bool operator<(const EseqSubjectDemuxKey & v2) const {
                        if (eseq < v2.eseq)
                            return true;
                        else
                            return false;
                    }

                    bool operator==(const EseqSubjectDemuxKey & v2) const {
                        return eseq == v2.eseq && subject == v2.subject;
                    }

                    EseqSubjectDemuxKey(const Headers<DCP> & header, const Subject_t & subj) :
                            eseq(header.eseq.occurs() ?
                                 header.eseq.get_eseq() :
                                 0xffffffff),
                            subject(subj),
                            eseq_header(header.eseq.occurs()) {
                    }
                };



                /*!
                 * \brief Policy: Event demultiplexer supporting event sequence numbers
                 *
                 * Alternatives: SingleEventDemux, MultiSourceDemux
                 */
                template <class DCP>
                class EventSeqDemux {

                        typedef typename DCP::SizeProp::elen_t   elen_t;
                        typedef typename DCP::SizeProp::flen_t   flen_t;
                        typedef typename DCP::SizeProp::fcount_t fcount_t;

                        typedef typename DCP::EventDemuxKey KeyType;
                        typedef typename DCP::Allocator Allocator;

                    public:

                        typedef EventSeqHeaderSupport<DCP> EventSeqHeaderPolicy;

                    private:

                        /*!
                         * \brief Internal event representation
                         * \tparam KeyType Type of key used to distinguish events arriving concurrently
                         */
                        template <class KeyType>
                        class Event {

                                typedef typename DCP::SizeProp::flen_t   flen_t;

                            public:
                                /// Defragmenter of the event
                                // TODO: def nicht mehr mit new, sondern free_resources() funktion, bei keep event mit aufgehoben
                                Defragmenter<DCP> * def;

                                /// Unique key identifying the event (equal to map key)
                                const KeyType key;

                                /// Status of the event
                                enum {
                                    /// Event not yet reconstructable. Still waiting for fragments.
                                    event_incomplete,
                                    /// Event reconstructed. Event sequence number locked to detect duplicates.
                                    event_outdated
                                } status;

                                /*!
                                 * \brief Absolute time when event will be dropped.
                                 *
                                 * If status == event_outdated it is time after which this is removed from events map
                                 * and thus sequence number is free to be used again.
                                 * If status == event_incomplete it is the time after which this event is dropped,
                                 * removed from event map and freed.
                                 */
                                shared::Time expire_time;


                                /// Constructor
                                Event(flen_t max_payload, const KeyType & key) :
                                        def(new (Allocator()) Defragmenter<DCP>(max_payload)),
                                        key(key), status(def ? event_incomplete : event_outdated) {
                                    touch();
                                }

                                /// Destructor
                                ~Event() {
                                    if (def)
                                        Allocator::destroy(def);
                                }

                                /*!
                                 * \brief Avoid incomplete event to be dropped because of timeout.
                                 */
                                void touch() {
                                    // Postpone time to drop incomplete fragment (3 seconds from now)
                                    shared::Time::get_current_time(expire_time);
                                    expire_time.add_sec(3);
                                }

                                /// Return key
                                const KeyType & get_key() {
                                    return key;
                                }

                                /// Returns handle of this event
                                void * to_handle() {
                                    return reinterpret_cast<void *>(this);
                                }

                                /// Returns event from handle
                                static Event * from_handle(void * handle) {
                                    return reinterpret_cast<Event *>(handle);
                                }
                        };

                        /// Maximum transmission unit (header + payload)
                        flen_t mtu;

                        typedef detail::PointerMap <
                                    KeyType,
                                    Event<KeyType>,
                                    (DCP::concurrent_events == (unsigned int)dynamic || DCP::old_event_ids == (unsigned int)dynamic ?
                                        dynamic :
                                        DCP::concurrent_events + DCP::old_event_ids)
                                > EventMap;

                        typedef detail::Queue <
                                    Event<KeyType> *,
                                    DCP::old_event_ids
                                > OutdatedQueue;

                        /// Assigns events to event keys (sequence numbers and/or subject)
                        EventMap events;

                        /// Completed/processed and dropped events (sorted by expire_time)
                        OutdatedQueue outdated_events;

                        /*!
                         * \brief Makes event outdated (event processed or dropped)
                         *
                         * Caller is responsible for deleting def.
                         */
                        void set_event_outdated(Event<KeyType> * e) {
                            e->def = 0;

                            if (!e->key.eseq_header) {
                                // Event without event sequence number -> no outdating, free immediately
                                events.erase(e->key);
                                Allocator::destroy(e);
                            } else {
                                // Default:
                                // Keep event sequence number for some time to detect late duplicates.
                                e->status = Event<KeyType>::event_outdated;

                                shared::Time::get_current_time(e->expire_time);

                                // Use time for cleaning outdated events
                                clean_outdated_events(e->expire_time);

                                // If the queue is full delete the oldest entry (at least)
                                if (outdated_events.full())
                                    clean_outdated_events(outdated_events.front()->expire_time);

                                e->expire_time.add_sec(3);
                                outdated_events.push_back(e);
                            }
                        }

                        /*!
                         * \brief Remove all outdated events with expired drop time
                         *
                         * Function is called from set_event_outdated to save syscall for getting current time.
                         */
                        void clean_outdated_events(const shared::Time & curr_time) {
                            while (!outdated_events.empty()) {
                                Event<KeyType> * e = outdated_events.front();

                                if (!(e->expire_time < curr_time))
                                    break;

                                ::logging::log::emit< ::logging::Info>()
                                    << PROGMEMSTRING("AFP: clean outdated event ")
                                    << ::logging::log::dec
                                    << (unsigned int)e->key.eseq
                                    << PROGMEMSTRING(" (seq can occur again)")
                                    << ::logging::log::endl;
                                events.erase(e->key);
                                Allocator::destroy(e);
                                outdated_events.pop();
                            }
                        }

                        /*!
                         * \brief Remove all incomplete events with expired drop time (expensive! not thread safe!)
                         */
                        void clean_incomplete_untouched_events() {
                            shared::Time curr_time;
                            Event<KeyType> * e;

                            shared::Time::get_current_time(curr_time);

                            typename EventMap::iterator it = events.begin();
                            while (it != events.end()) {
                                e = *it;
                                if (e->status == Event<KeyType>::event_incomplete && e->expire_time < curr_time) {
                                    ::logging::log::emit< ::logging::Info>()
                                        << PROGMEMSTRING("AFP: clean untouched event (timeout)")
                                        << ::logging::log::endl;
                                    events.erase(it++);
                                    Allocator::destroy(e);
                                } else {
                                    ++it;
                                }
                            }
                        }

                    public:

                        /// Constructor
                        EventSeqDemux(flen_t mtu)
                                : mtu(mtu) {
                        }

                        /// Destructor
                        ~EventSeqDemux() {
                            typename EventMap::iterator it = events.begin();
                            for (; it != events.end(); ++it)
                                Allocator::destroy(*it);
                        }

                        /*!
                         * \returns Defragmenter handle, zero to drop the fragment.
                         */
                        void * get_defragmenter_handle(const Headers<DCP> & header, const KeyType & event_key) {
                            // Call cleanup function for every 10th fragment (TODO: place this somewhere else... quite expensive)
                            static int call = 0;
                            if (++call % 10 == 0)
                                clean_incomplete_untouched_events();

                            typename EventMap::iterator it = events.find(event_key);
                            Event<KeyType> * event;

                            if (it == events.end()) {
                                // Unknown key
                                // -> create new event defragmenter
                                FAMOUSO_ASSERT(mtu > header.length());
                                event = new (Allocator()) Event<KeyType>(mtu - header.ext_length(), event_key);
                                if (!event || event->status == Event<KeyType>::event_outdated || !events.insert(event)) {
                                    ::logging::log::emit< ::logging::Warning>()
                                        << PROGMEMSTRING("AFP: Out of memory -> drop")
                                        << ::logging::log::endl;
                                    return 0;
                                }
                                ::logging::log::emit< ::logging::Info>()
                                    << PROGMEMSTRING("AFP: defrag fragment ")
                                    << ::logging::log::dec << (unsigned int)header.fseq
                                    << PROGMEMSTRING(" of NEW event ")
                                    << (unsigned int)event_key.eseq << ::logging::log::endl;
                                return event->to_handle();
                            }

                            // Found event
                            event = *it;
                            FAMOUSO_ASSERT(event->key == event_key);

                            if (event->status == Event<KeyType>::event_outdated || event->def->is_event_complete()) {
                                // Event was already processed, dropped or is
                                // complete (late fragment of already dropped
                                // event, duplicate or FEC redundancy fragment
                                // not needed) -> drop fragment
                                ::logging::log::emit< ::logging::Info>()
                                    << PROGMEMSTRING("AFP: dropping outdated fragment ")
                                    << ::logging::log::dec << (unsigned int)header.fseq
                                    << PROGMEMSTRING(" of event ")
                                    << (unsigned int)event_key.eseq << ::logging::log::endl;
                                return 0;
                            }

                            // Incomplete event will be dropped if it is not been touched for 3 seconds
                            // Do not touch for every fragment to reduce frequency of expensive gettimeofday() system calls
                            if (header.eseq.occurs() && (header.eseq.get_eseq() & 3) == 0)
                                event->touch();

                            ::logging::log::emit< ::logging::Info>()
                                << PROGMEMSTRING("AFP: defrag fragment ")
                                << ::logging::log::dec << (unsigned int)header.fseq
                                << PROGMEMSTRING(" of event ")
                                << (unsigned int)event_key.eseq << ::logging::log::endl;
                            return event->to_handle();
                        }

                        /// Return Defragmenter from handle
                        Defragmenter<DCP> * get_defragmenter(void * handle) {
                            return Event<KeyType>::from_handle(handle)->def;
                        }

                        /*!
                         * \brief Frees defragmenter.
                         * \param handle Defragmenter's handle
                         *
                         * After calling keep_defragmenter(handle) use free_kept_defragmenter()
                         * instead of this.
                         */
                        void free_defragmenter(void * handle) {
                            Event<KeyType> * e = Event<KeyType>::from_handle(handle);
                            Allocator::destroy(e->def);
                            set_event_outdated(e);
                        }


                        /// Marks this policy to support late delivery of defragmented events (defragmenter
                        /// not freed immediately after event is complete)
                        enum { support_late_delivery };

                        /*!
                         * \brief Keep defragmenter for later event delivery and return its new handle.
                         * \param handle Defragmenter's handle
                         * \return New handle you must pass to free_kep_defragmenter
                         * \pre Event must be complete.
                         *
                         * After using this function use free_kept_defragmenter()
                         * instead of free_defragmenter().
                         */
                        void * keep_defragmenter(void * handle) {
                            // Keep Defragmenter instance seperated from Event marked as processed.
                            // It will be deleted in free_kept_defragmenter.
                            Event<KeyType> * e = Event<KeyType>::from_handle(handle);
                            Defragmenter<DCP> * def = e->def;
                            FAMOUSO_ASSERT(def->is_event_complete());
                            set_event_outdated(e);
                            return static_cast<void *>(def);
                        }

                        /*!
                         * \brief Get defragmenter from kept defragmenter's handle.
                         * \param handle Handle returned by keep_defragmenter.
                         * \return Defragmenter
                         */
                        Defragmenter<DCP> * get_kept_defragmenter(void * handle) {
                            return static_cast<Defragmenter<DCP> *>(handle);
                        }

                        /*!
                         * \brief Frees kept defragmenter.
                         * \param handle Defragmenter's handle
                         *
                         * After calling keep_defragmenter(handle) use this function
                         * instead of free_defragmenter().
                         */
                        void free_kept_defragmenter(void * handle) {
                            Allocator::destroy(get_kept_defragmenter(handle));
                        }

                };


            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __EVENTSEQDEMUX_H_2A8B3BD61E7528__

