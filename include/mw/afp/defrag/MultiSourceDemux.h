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


#ifndef __MULTISOURCEDEMUX_H_2A8B3BD61E7528__
#define __MULTISOURCEDEMUX_H_2A8B3BD61E7528__


#include "debug.h"

#include "mw/afp/defrag/Defragmenter.h"
#include "mw/afp/defrag/detail/PointerMap.h"
#include "mw/afp/defrag/NoEventSeqHeaderSupport.h"


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {


                /*!
                 * \brief Demux key type with multiple subjects and one publisher per subject, no event sequence numbers
                 */
                template <typename AFPDC, class Subject_t>
                struct SubjectDemuxKey {
                    const Subject_t subject;

                    enum { uses_subject = 1 };

                    bool operator<(const SubjectDemuxKey & v2) const {
                        return subject < v2.subject;
                    }

                    bool operator==(const SubjectDemuxKey & v2) const {
                        return subject == v2.subject;
                    }

                    SubjectDemuxKey(const Headers<AFPDC> & header, const Subject_t & subj) :
                            subject(subj) {
                    }
                };



                // TODO: Testing!!!
                /*!
                 * \brief Policy: Event demultiplexer that can handle events from multiple sources,
                 *                but only one event per source simultaneuosly
                 *
                 * IMPORTANT: This demux policy cannot handle multiple arriving events at one time.
                 * Thus multithreading will cause problems if threads share one DefragmentationProcessor!
                 * You also have to ensure that a received event's data is not needed anymore when
                 * next event's fragments are going to be processed!
                 *
                 * Use this policy for channels without packet loss, reordering and late duplicates or
                 * if you are sure that fragments of only one event per source will be received during
                 * defragmentation.
                 * This policy is a little robust concerning packet loss, but may lead to erroneous
                 * events if first fragment is lost.
                 *
                 * This policy does not use event sequence numbers.
                 *
                 * Alternatives: SingleEventDemux, EventSeqDemux
                 */
                template <class AFPDC>
                class MultiSourceDemux {

                        typedef typename AFPDC::SizeProp::elen_t   elen_t;
                        typedef typename AFPDC::SizeProp::flen_t   flen_t;
                        typedef typename AFPDC::SizeProp::fcount_t fcount_t;

                        typedef typename AFPDC::EventDemuxKey KeyType;
                        typedef typename AFPDC::Allocator Allocator;

                    public:

                        typedef NoEventSeqHeaderSupport<AFPDC> EventSeqHeaderPolicy;

                    private:

                        /*!
                         * \brief Internal event representation
                         * \tparam KeyType Type of key used to distinguish events arriving concurrently
                         */
                        template <class KeyType>
                        class Event {

                                typedef typename AFPDC::SizeProp::flen_t   flen_t;

                            public:
                                /// Defragmenter of the event
                                Defragmenter<AFPDC> def;

                                /// Unique key identifying the event (equal to map key)
                                KeyType key;

                                /// Constructor
                                Event(flen_t max_payload, const KeyType & key)
                                        : def(max_payload), key(key) {
                                }

                                /// Destructor
                                ~Event() {
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

                        typedef detail::PointerMap < KeyType, Event<KeyType>, 100 /*TODO*/ > EventMap;

                        /// Assigns events to event keys (sequence numbers and/or subject and/or publisher address <- TODO)
                        EventMap events;

                    public:

                        /// Constructor
                        MultiSourceDemux(flen_t mtu)
                                : mtu(mtu) {
                        }

                        /// Destructor
                        ~MultiSourceDemux() {
                            typename EventMap::iterator it = events.begin();
                            Event<KeyType> * event;
                            for (; it != events.end(); it++) {
                                event = *it;
                                Allocator::destroy(event);
                            }
                        }

                        /*!
                         * \brief Return defragmenter handle of fragment
                         * \param header Header of the fragment
                         * \param event_key Event key to distingush events
                         * \returns Defragmenter handle, zero to drop the fragment.
                         */
                        void * get_defragmenter_handle(const Headers<AFPDC> & header, const KeyType & event_key) {
                            typename EventMap::iterator it = events.find(event_key);
                            Event<KeyType> * event;

                            // Start new defragmentation only with first fragment.
                            if (header.first_fragment) {
                                // First fragment!

                                // Improve robustness:
                                // Channel is assumed to be ideal, so old defragmenters should be
                                // freed when a new first fragment arrives. But in case there are
                                // packet loss or reordering and current event is not complete while
                                // we get first fragment of the next event drop incomplete event.
                                if (it != events.end()) {
                                    free_defragmenter(event->to_handle());
                                }

                                // Unknown or recently freed key
                                // -> create new event defragmenter
                                FAMOUSO_ASSERT(mtu > header.length());
                                event = new (Allocator()) Event<KeyType>(mtu - header.length(), event_key);

                                if (!event)
                                    goto out_of_mem_error;
                                if (!events.insert(event))
                                    goto out_of_mem_error;

                                ::logging::log::emit< ::logging::Info>()
                                        << "AFP: defrag fragment " << ::logging::log::dec << (unsigned int)header.fseq
                                        << " of NEW event " << (unsigned int)event_key.eseq << ::logging::log::endl;
                            } else {
                                // Not first fragment: If we found a defragmenter fitting the key,
                                // return it. Otherwise (missing frist fragment) drop this fragment.

                                // Improve robustness:
                                // Channel is assumed to be ideal, so first fragment should arrive
                                // first. If it did not (event defragmenter not found), drop other
                                // fragments.
                                if (it == events.end())
                                    return 0;

                                event = *it;

                                ::logging::log::emit< ::logging::Info>()
                                        << "AFP: defrag fragment " << ::logging::log::dec << (unsigned int)header.fseq
                                        << " of event " << (unsigned int)event_key.eseq << ::logging::log::endl;
                            }

                            return event->to_handle();

                        out_of_mem_error:
                            ::logging::log::emit< ::logging::Warning>() << "AFP: Out of memory -> drop" << ::logging::log::endl;
                            return 0;
                        }

                        /// Return Defragmenter from handle
                        Defragmenter<AFPDC> * get_defragmenter(void * handle) {
                            return Event<KeyType>::from_handle(handle)->def;
                        }

                        /*!
                         * \brief Frees defragmenter.
                         * \param handle Defragmenter's handle
                         */
                        void free_defragmenter(void * handle) {
                            Event<KeyType> * e = Event<KeyType>::from_handle(handle);
                            events.erase(e->get_key());
                            Allocator::destroy(e);
                        }
                };


            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __MULTISOURCEDEMUX_H_2A8B3BD61E7528__

