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


#ifndef __DUPLICATECHECKER_H_D61F11F89E744A__
#define __DUPLICATECHECKER_H_D61F11F89E744A__


#if !defined(__AVR__)
// Non-AVR version

#include <vector>


namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {


                /*!
                 * \brief Policy: Checking for duplicate fragment packages
                 *
                 * If the network can produce duplicates, use this policy.

                 * Alternative policies: NoDuplicateChecker
                 */
                template <class DCP>
                class DuplicateChecker {

                        /// Which fragments already arrived?
                        std::vector<bool> fragment_arrived;

                    public:

                        /*!
                         * \brief Check if this fragment is duplicate
                         * \param header AFP header of the fragment
                         * \returns True if this is a new fragment. False if it's a duplicate.
                         */
                        bool new_fragment(const Headers<DCP> & header) {
                            if (fragment_arrived.size() <= header.fseq) {
                                // This will be called as rarely as reordering happens, because
                                // fragments with highest fseq are sent first.
                                fragment_arrived.resize(header.fseq + 1);
                            }

                            if (fragment_arrived[header.fseq] == true) {
                                return false;
                            } else {
                                fragment_arrived[header.fseq] = true;
                                return true;
                            }
                        }
                };


            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso


#else
// AVR version

#include "boost/mpl/assert.hpp"

namespace famouso {
    namespace mw {
        namespace afp {
            namespace defrag {
                template <class DCP>
                class DuplicateChecker {
                    BOOST_MPL_ASSERT_MSG(false, Duplicate_checking_currently_not_supported_on_AVR, ());
                };
            } // namespace defrag
        } // namespace afp
    } // namespace mw
} // namespace famouso

#endif


#endif // __DUPLICATECHECKER_H_D61F11F89E744A__

