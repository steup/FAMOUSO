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

#ifndef _HasLessThanRelation_h_
#define _HasLessThanRelation_h_

#include "mw/attributes/detail/tags/Tag.h"

#include "boost/mpl/if.hpp"

#include "mw/attributes/filter/less_than_or_equal_to.h"
#include "mw/attributes/filter/greater_than_or_equal_to.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

                /*!
                 * \brief Attribute tag deciding the stronger-weaker-relation
                 *  of the attribute
                 */
                struct HasLessThanRelation : public Tag {
                    /*!
                     * \brief Extracts the attribute comparator (i.e. the stronger-
                     *  weaker-relation) of an attribute from the given tag set.
                     *
                     * To perform this the given tag set is checked for containing
                     *  the HasLessThanRelation attribute tag. If it does a comparator
                     *  representing a less-than-relation is returned. Otherwise a
                     *  comparator for a greater-than-relation is returned. Each
                     *  comparator also covers the equal-to case.
                     */
                    template <typename TagSet>
                    struct get_comparator {
                            typedef typename boost::mpl::if_<
                                              typename TagSet::template contains_tag<HasLessThanRelation>,

                                              famouso::mw::attributes::filter::less_than_or_equal_to,

                                              famouso::mw::attributes::filter::greater_than_or_equal_to
                                             >::type type;
                    };
                };

            } // end namespace detail
        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif // _HasLessThanRelation_h_
