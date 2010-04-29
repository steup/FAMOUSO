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

#ifndef _Find_Static_h_
#define _Find_Static_h_

#include "boost/mpl/end.hpp"
#include "boost/mpl/find_if.hpp"
#include "boost/mpl/eval_if.hpp"
#include "boost/type_traits/is_same.hpp"

#include "mw/attributes/type_traits/is_same_base_type.h"

namespace famouso {
    namespace mw {
        namespace attributes {
            namespace detail {

                /*!
                 * \brief Searches for the given attribute in the given attribute
                 *  sequence and determines whether it is contained or not.
                 *
                 * \tparam Attr The attribute to search for
                 * \tparam AttrSeq The attribute sequence to search in
                 */
                template <typename Attr, typename AttrSeq>
                struct Find {
                    private:
                        typedef typename boost::mpl::find_if<
                                                      AttrSeq,
                                                      type_traits::is_same_base_type<
                                                                    Attr,
                                                                    boost::mpl::_1
                                                                   >
                                                     >::type findIter;

                    public:
                        typedef typename boost::mpl::eval_if<
                                                      boost::is_same<
                                                       findIter,
                                                       typename boost::mpl::end<AttrSeq>::type
                                                      >,
                                                      boost::mpl::bool_<false>,
                                                      boost::mpl::bool_<true>
                                                     >::type result;
                };

            } // end namespace detail
        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif
