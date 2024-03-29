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


#ifndef __DEFRAGMENTER_H_B2FF50B5F3CB6D__
#define __DEFRAGMENTER_H_B2FF50B5F3CB6D__


#include "debug.h"


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {


                template <class DCP> class Headers;


                /*!
                 * \brief Reconstructs one event from fragments
                 *
                 * Includes fragment duplicate checking (if configured by policy) and event data reconstruction.
                 */
                template <class DCP>
                class Defragmenter {

                        typedef typename DCP::SizeProp::elen_t elen_t;
                        typedef typename DCP::SizeProp::flen_t flen_t;
                        typedef typename DCP::SizeProp::fcount_t fcount_t;

                        typedef class DCP::EventDataReconstructionPolicy EventDataReconstructionPolicy;
                        typedef class DCP::DuplicateCheckingPolicy DuplicateCheckingPolicy;

                        typedef class DCP::DefragStatistics Statistics;

                    private:

                        /// Event reconstruction
                        EventDataReconstructionPolicy event_reconst;

                        /// Fragment duplicate checking
                        DuplicateCheckingPolicy dup_check;

                    public:

                        /*!
                         * \brief Construct a new event to be defragmented.
                         * \param max_chunk_length Maximum length of one fragments data
                         */
                        Defragmenter(flen_t max_chunk_length)
                                : event_reconst(max_chunk_length) {
                        }

                        /*!
                         * \brief Process a fragment known to be part of this event
                         *
                         * The fragment passed must be part of the event which is reconstructed
                         * by this instance. The demultiplexing policy should ensure that.
                         *
                         * Includes duplicate checking if you do not use NoDuplicateChecking policy.
                         */
                        void put_fragment(const Headers<DCP> & header, const uint8_t * data, flen_t length) {
                            // Is this fragment a duplicate?
                            if (!dup_check.new_fragment(header)) {
                                Statistics::fragment_duplicate();
                                ::logging::log::emit< ::logging::Info>()
                                    << PROGMEMSTRING("AFP: Dropped duplicate fragment")
                                    << ::logging::log::endl;
                                return;
                            }

                            event_reconst.put_fragment(header, data, length);
                        }

                        /*!
                         * \brief Returns whether the event already got reconstructed completely.
                         */
                        bool is_event_complete() {
                            return event_reconst.is_complete();
                        }

                        /*!
                         * \brief Get data of the reconstructed event.
                         * \pre is_event_complete() returns true
                         */
                        uint8_t * get_event_data() {
                            return event_reconst.get_data();
                        }

                        /*!
                         * \brief Get length of the reconstructed event.
                         * \pre is_event_complete() returns true
                         */
                        elen_t get_event_length() {
                            return event_reconst.get_length();
                        }

                        /*!
                         * \brief True if the event cannot be reconstructed anymore for sure.
                         */
                        bool error() {
                            return event_reconst.error();
                        }
                };


            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __DEFRAGMENTER_H_B2FF50B5F3CB6D__

