/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
 *                    Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __AFPCONFIG_H_6E4D6E109554C6__
#define __AFPCONFIG_H_6E4D6E109554C6__

#include "mw/afp/Config.h"

namespace famouso {
    namespace mw {
        namespace nl {

            template< class CAN_Driver, class CCP, class BP >
            class CANNL;

            class voidNL;

        } // namespace nl


        namespace anl {

            /*!
             *  \brief  Base class for all ANL AFP configs
             *  \tparam SNN     type to distinguish between different
             *                  fragment sources (subjects)
             */
            template < typename SNN >
            struct AFPBaseConfig : afp::DefaultConfig {
                enum {
                    multiple_subjects = true
                };
                typedef SNN SubjectType;
            };

            /*!
             *  \brief  Default ANL AFP config (all network layers without specialization)
             */
            template < class NL >
            struct AFPConfig {
                struct type : AFPBaseConfig<typename NL::SNN> {
                    enum {
                        event_seq = true,
                        reordering = true,
                        duplicates = true
                    };
                };
            };

            /*!
             *  \brief  Default ANL AFP config (CANNL)
             */
            template < class CD, class CCP, class BP>
            struct AFPConfig< nl::CANNL<CD,CCP,BP> > {
                typedef AFPBaseConfig<typename nl::CANNL<CD,CCP,BP>::SNN> type;
            };

            /*!
             *  \brief  Default ANL AFP config (voidNL)
             */
            template <>
            struct AFPConfig<nl::voidNL> {
                typedef afp::Disable type;
            };

        } // namespace anl
    } // namespace mw
} //namespace famouso

#endif // __AFPCONFIG_H_6E4D6E109554C6__

