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


#ifndef __SINGLEEVENTDEMUX_H_068FBC9538BBE3__
#define __SINGLEEVENTDEMUX_H_068FBC9538BBE3__


#include "object/PlacementNew.h"
#include "mw/afp/defrag/Defragmenter.h"

#include "mw/afp/defrag/NoEventSeqHeaderSupport.h"


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {



                /*!
                 * \brief Demux key type with one subject and one publisher, no event sequence number
                 */
                template <typename DCP, class Subject_t>
                struct EmptyDemuxKey {
                    enum { uses_subject = 0 };

                    EmptyDemuxKey(const Headers<DCP> & header, const Subject_t & subj) {
                    }
                };



                // TODO: testing
                /*!
                 * \brief Policy: Simple demultiplexer that can handle only one event simultaneously
                 *
                 * IMPORTANT: This demux policy cannot handle multiple arriving events at one time.
                 * Thus multithreading will cause problems if threads share one DefragmentationProcessor!
                 * You also have to ensure that a received event's data is not needed anymore when
                 * next event's fragments are going to be processed!
                 *
                 * Use this policy for channels without packet loss, reordering and late duplicates or
                 * if you are sure that fragments of only one event will be received during defragmentation.
                 * This policy is a little robust concerning packet loss, but may lead to erroneous
                 * events if first fragment is lost.
                 *
                 * This policy does not use event sequence numbers.
                 *
                 * Alternatives: MultiSourceDemux, EventSeqDemux
                 */
                template <class DCP>
                class  SingleEventDemux {

                        typedef typename DCP::SizeProp::elen_t   elen_t;
                        typedef typename DCP::SizeProp::flen_t   flen_t;
                        typedef typename DCP::SizeProp::fcount_t fcount_t;

                        typedef typename DCP::EventDemuxKey KeyType;

                    public:

                        typedef NoEventSeqHeaderSupport<DCP> EventSeqHeaderPolicy;

                    private:

                        /// MTU of fragments (maximum size including header)
                        flen_t mtu;

                        /// Memory for one defragmenter object
                        uint8_t _defragmenter[sizeof(Defragmenter<DCP>)];

                        /// Defragmenter of currently received event
                        Defragmenter<DCP> * curr_defrag;

                    public:

                        /// Constructor
                        SingleEventDemux(flen_t mtu)
                                : mtu(mtu), curr_defrag(0) {
                        }

                        /// Destructor
                        ~SingleEventDemux() {
                            if (curr_defrag)
                                curr_defrag->~Defragmenter<DCP>();
                        }

                        // Event must have been processed before you can start receiving a new event.
                        void * get_defragmenter_handle(const Headers<DCP> & header, const KeyType & key) {
                            // Start new defragmentation only with first fragment.
                            if (header.first_fragment) {
                                // First fragment!

                                // Improve robustness:
                                // Channel is assumed to be perfect, so old defragmenters should be
                                // freed when a new first fragment arrives. But in case there are
                                // packet loss or reordering and current event is not complete while
                                // we get first fragment of the next event drop incomplete event.
                                if (curr_defrag) {
                                    free_defragmenter(curr_defrag);
                                }

                                // Start new defragmentation
                                FAMOUSO_ASSERT(mtu > header.length());
                                curr_defrag = new(_defragmenter) Defragmenter<DCP>(mtu - header.ext_length());
                            } else {
                                // Not first fragment: if we are defragmenting an event return
                                // the current defragmenter, otherwise drop this fragment by
                                // returning zero.
                            }

                            return curr_defrag;
                        }

                        Defragmenter<DCP> * get_defragmenter(void * handle) {
                            return curr_defrag;
                        }

                        // Event must have been processed before you can start receiving a new event.
                        void free_defragmenter(void * handle) {
                            curr_defrag->~Defragmenter<DCP>();
                            curr_defrag = 0;
                        }

                };


            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __SINGLEEVENTDEMUX_H_068FBC9538BBE3__

