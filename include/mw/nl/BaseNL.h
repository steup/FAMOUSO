/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *                    2010 Marcus Foerster <MarcusFoerster1@gmx.de>
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

#ifndef _BaseNL_h_
#define _BaseNL_h_

#include "mw/nl/BaseNL.h"

#include "boost/mpl/aux_/na.hpp"

#include "mw/attributes/detail/SetProvider.h"
#include "mw/attributes/AttributeSet.h"

namespace famouso {
    namespace mw {
        namespace nl {

            /*!
             * \brief This network layer classer extends the DistinctNL by adding
             *  an attribute provision.
             *
             * The provision provides as an AttributeSet type named attributeProvision
             *  should reflect the abilities of this network layer, i.e. which latency
             *  can be guaranteed or which maximum bandwidth is possible.
             * If nothing can be guaranteed at all, an empty list should be passed, that
             *  is the all template parameters should all be omitted.
             *
             * \tparam A1orSeq The first attribute or a sequence of attributes
             * \tparam A2 The second attribute
             * \tparam A3 The third attribute
             * \tparam A4 The fourth attribute
             * \tparam A5 The fifth attribute
             * \tparam A6 The sixth attribute
             * \tparam A7 The seventh attribute
             * \tparam A8 The eighth attribute
             * \tparam A9 The ninth attribute
             * \tparam A10 The tenth attribute
             */
            template <typename A1orSeq = boost::mpl::na, typename A2 = boost::mpl::na,
                      typename A3 = boost::mpl::na, typename A4 = boost::mpl::na,
                      typename A5 = boost::mpl::na, typename A6 = boost::mpl::na,
                      typename A7 = boost::mpl::na, typename A8 = boost::mpl::na,
                      typename A9 = boost::mpl::na, typename A10 = boost::mpl::na>
            class BaseNL : public DistinctNL {
                public:
                    /*!
                     * \brief The provided attributes of this network layer.
                     */
                    typedef typename famouso::mw::attributes::detail::SetProvider<
                                                                       A1orSeq, A2, A3, A4,
                                                                       A5, A6, A7, A8, A9, A10
                                                                      >::attrSet attributeProvision;
            };


        }  // namespace nl
    }  // namespace mw
}  // namespace famouso

#endif
