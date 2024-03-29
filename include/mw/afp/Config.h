/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
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


#ifndef __CONFIG_H_6D03524A376067__
#define __CONFIG_H_6D03524A376067__


namespace famouso {
    namespace mw {
        namespace afp {

            enum {
                /*!
                 *  \brief  Constant for choosing dynamic data structures
                 *
                 *  Several data structures needed in some AFP configurations
                 *  can be configured either to a constant size limiting the
                 *  maximum value of elements, or to a dynamic size requiring
                 *  memory allocations. See the configuration options
                 *  \ref afp_config_options_concurrent_events,
                 *  \ref afp_config_options_old_event_ids and
                 *  \ref afp_config_options_max_fragments.
                 */
                dynamic = 0
            };

            /*!
             *  \brief  Config: default AFP config
             */
            struct DefaultConfig {
                enum {
                    event_seq = false,
                    reordering = false,
                    duplicates = false,
                    FEC = false,                        // defrag: support
                    overflow_error_checking = true,

                    // Defrag only

                    multiple_subjects = false,
                    defrag_statistics = false,

#if defined(__NO_STL__)
                    concurrent_events = 8,      // multiple_subjects or event_seq
                    old_event_ids = 8,          // event_seq
                    max_fragments = 256         // duplicates
#else
                    concurrent_events = dynamic,     // multiple_subjects or event_seq
                    old_event_ids = dynamic,         // event_seq
                    max_fragments = dynamic         // duplicates
#endif

                };
            };


            /*!
             *  \brief  Config: disable fragmentation/defragmentation
             *                  support  completely
             */
            struct Disable {};

            /// Config: self-documenting synonym of Disable
            typedef Disable NoFragmentation;

            /// Config: self-documenting synonym of Disable
            typedef Disable NoDefragmentation;


            /*!
             *  \brief  Config: default with multiple subject support
             *  \tparam SNN     type to distinguish between different
             *                  subjects
             */
            template < typename SNN >
            struct MultiSubjectConfig : afp::DefaultConfig {
                enum {
                    multiple_subjects = true
                };
                typedef SNN SubjectType;
            };

            /*!
             *  \brief  Config: default with multiple subject support,
             *                  event sequence numbering, support to handle
             *                  package reordering and duplicate checking
             *  \tparam SNN     type to distinguish between different
             *                  subjects
             */
            template < typename SNN >
            struct MultiSubjectESeqReorderDuplicateConfig : afp::MultiSubjectConfig<SNN> {
                enum {
                    event_seq = true,
                    reordering = true,
                    duplicates = true
                };
            };


        } // namespace afp
    } // namespace mw
} // namespace famouso


#endif // __CONFIG_H_6D03524A376067__

