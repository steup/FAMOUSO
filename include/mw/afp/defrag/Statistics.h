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

#ifndef __STATISTICS_H_2F8C97C9EC89D7__
#define __STATISTICS_H_2F8C97C9EC89D7__


#include <string.h>

#ifndef __AVR__
#include "boost/thread/mutex.hpp"
#endif


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {


                /*!
                 * \brief Statistics about processed fragments
                 *
                 * The number of lost fragments can be approximated:
                 *
                 *  <tt>lost = #expected - #currently_expected - #received</tt>
                 *
                 * For short times the result may be slightly higher than the real value. The maximum difference is
                 * equal to the number of expected redundancy fragments not yet received.
                 *
                 * \attention At the moment there is no overflow handling. Thus the statistics
                 *   may be inconsistent if some values overflow while others do not yet.
                 * \see EventStats, Statistics, NoStatistics
                 */
                struct FragmentStats {
                    /// Total fragment number of all (at least partially) received events. Increased when first fragment of an event is processed.
                    unsigned int expected;

                    /// Total number of fragments not received yet for all incomplete events. Increased when first fragment of an event is processed. Decreased for every processed fragment. After we are done with an event the value has been increased and decreased by the same number.
                    unsigned int currently_expected;



                    /// Total number of all received fragments. Incremented when a fragment gets processed. #received = #incorrect_header + #outdated + #duplicates + #used
                    unsigned int received;

                    /// Total number of fragments that have be dopped because the header contains errors or uses features / size properties not available in the used AFP defrag configuration.
                    unsigned int incorrect_header;

                    /// Total number of outdated fragments. This includes fragments belonging to events which have already been reconstructed (late duplicates, redundancy fragments not needed) or dropped because of the timeout.
                    unsigned int outdated;

                    /// Total number of fragments used for reconstruction of events. Incremented during fragment processing.
                    unsigned int used;

                    /// Total number of duplicates received during reconstruction of the events the duplicates belong to. Incremented during duplicate checking (if configured to check for duplicates).
                    unsigned int duplicates;

                    /// Constructor (init to zero)
                    FragmentStats() {
                        reset();
                    }

                    /// Reset statistics to zero
                    void reset() {
                        memset(this, 0, sizeof(*this));
                    }
                };


                /*!
                 * \brief Statistics about reconstructed events
                 *
                 * \attention At the moment there is no overflow handling. Thus the statistics
                 *   may be inconsistent if some values overflow while others do not yet.
                 * \see FragmentStats, Statistics, NoStatistics
                 */
                struct EventStats {

                    /// Number of events that have been reconstructed completely
                    unsigned int complete;

                    /// Number of events that have been dropped because we had to less fragments for reconstruction before timeout
                    unsigned int incomplete;

                    /// Constructor (init to zero)
                    EventStats() {
                        reset();
                    }

                    /// Reset statistics to zero
                    void reset() {
                        memset(this, 0, sizeof(*this));
                    }
                };



                /*!
                 * \brief Policy: Defragmentation statistics
                 * \see FragmentStats, EventStats, NoStatistics
                 */
                template <class Tag>
                class Statistics {
                        // TODO: mutex als policy

                        static FragmentStats & fragment() {
                            static FragmentStats s;
                            return s;
                        }

                        static EventStats & event() {
                            static EventStats s;
                            return s;
                        }

#ifndef __AVR__
                        static boost::mutex & mutex() {
                            static boost::mutex m;
                            return m;
                        }
#endif

                        static void inc(unsigned int & val) {
#ifndef __AVR__
                            mutex().lock();
                            val++;
                            mutex().unlock();
#else
                            val++;
#endif
                        }

                        static void add(unsigned int & val, unsigned int to_add) {
#ifndef __AVR__
                            mutex().lock();
                            val += to_add;
                            mutex().unlock();
#else
                            val += to_add;
#endif
                        }

                        static void sub(unsigned int & val, unsigned int to_sub) {
#ifndef __AVR__
                            mutex().lock();
                            val -= to_sub;
                            mutex().unlock();
#else
                            val -= to_sub;
#endif
                        }

                    public:

                        /// Increment total number of fragments received (called by DefragmentationProcessor)
                        static void fragment_received() {
                            inc(fragment().received);
                        }

                        /// Increment number of fragments with incorrect header (called by DefragmentationProcessor)
                        static void fragment_incorrect_header() {
                            inc(fragment().incorrect_header);
                        }

                        /// Increment total number of fragments ariving too late (called by DefragmentationProcessor)
                        static void fragment_outdated() {
                            inc(fragment().outdated);
                        }

                        /// Increment number of duplicate fragments (called by Defragmenter)
                        static void fragment_duplicate() {
                            inc(fragment().duplicates);
                        }

                        /// Increment number of used fragments (called by data reconstruction policy)
                        static void fragment_used() {
                            inc(fragment().used);
                        }

                        /// Increase number of expected fragments (called by data reconstruction policy)
                        static void fragments_expected(unsigned int n) {
                            add(fragment().expected, n);
                        }

                        /// Increase number of currently expected fragments (called by data reconstruction policy)
                        static void fragments_currently_expected_add(unsigned int n) {
                            add(fragment().currently_expected, n);
                        }

                        /// Decrease number of currently expected fragments (called by data reconstruction policy)
                        static void fragments_currently_expected_sub(unsigned int n) {
                            sub(fragment().currently_expected, n);
                        }


                        /// Increment number of reconstructed events (called by data reconstruction policy)
                        static void event_complete()   {
                            inc(event().complete);
                        }

                        /// Increment number of events not completely reconstructed (called by data reconstruction policy)
                        static void event_incomplete() {
                            inc(event().incomplete);
                        }


                        /// Get current statistics about fragments
                        static void get_fragment_stats(FragmentStats & fs) {
#ifndef __AVR__
                            mutex().lock();
                            memcpy(&fs, &fragment(), sizeof(fs));
                            mutex().unlock();
#else
                            memcpy(&fs, &fragment(), sizeof(fs));
#endif
                        }

                        /// Get current statistics about events
                        static void get_event_stats(EventStats & es) {
#ifndef __AVR__
                            mutex().lock();
                            memcpy(&es, &event(), sizeof(es));
                            mutex().unlock();
#else
                            memcpy(&es, &event(), sizeof(es));
#endif
                        }
                };



                /*!
                 * \brief Policy: No defragmentation statistics
                 * \see FragmentStats, EventStats, Statistics
                 */
                class NoStatistics {
                    public:

                        /// Increment total number of fragments received (called by DefragmentationProcessor)
                        static void fragment_received() {}

                        /// Increment number of fragments with incorrect header (called by DefragmentationProcessor)
                        static void fragment_incorrect_header() {}

                        /// Increment total number of fragments ariving too late (called by DefragmentationProcessor)
                        static void fragment_outdated() {}

                        /// Increment number of duplicate fragments (called by Defragmenter)
                        static void fragment_duplicate() {}

                        /// Increment number of used fragments (called by data reconstruction policy)
                        static void fragment_used() {}

                        /// Increase number of expected fragments (called by data reconstruction policy)
                        static void fragments_expected(unsigned int n) {}

                        /// Increase number of currently expected fragments (called by data reconstruction policy)
                        static void fragments_currently_expected_add(unsigned int n) {}

                        /// Decrease number of currently expected fragments (called by data reconstruction policy)
                        static void fragments_currently_expected_sub(unsigned int n) {}


                        /// Increment number of reconstructed events (called by data reconstruction policy)
                        static void event_complete()   {}

                        /// Increment number of events not completely reconstructed (called by data reconstruction policy)
                        static void event_incomplete() {}


                        /// Get current statistics about fragments
                        static void get_fragment_stats(FragmentStats & fs) {
                        }

                        /// Get current statistics about events
                        static void get_event_stats(EventStats & es) {
                        }
                };



            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __STATISTICS_H_2F8C97C9EC89D7__

