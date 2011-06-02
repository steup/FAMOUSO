/*******************************************************************************
 *
 * Copyright (c) 2011 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
 * $Id: $
 *
 ******************************************************************************/

#ifndef __ATTRIBUTESEQ_H_DD54D24E66A1EA__
#define __ATTRIBUTESEQ_H_DD54D24E66A1EA__

#include "mw/attributes/detail/AttributeSetProvider.h"

 namespace famouso {
    namespace mw {
        namespace attributes {

           /*!  \brief Represents the interface class of a duplicate free
            *          sequence of %attributes usable for compile time
            *          computations and during runtime as binary representation
            *
            *   The struct provides 10 type arguments which can either all be
            *   attributes or only the first of them a forward sequence as
            *   defined by the Boost MPL library.
            *
            *   A compile time assertion in an inner implementation assures
            *   that only these two cases can be instantiated.
            *
            *   It is also legal to provide none of the template parameters at
            *   all.
            *
            *   Both variants lead to an AttributeSequence type defined
            *   containing either the single attributes or the elements of the
            *   provided sequence.
            *
            *   \tparam A1 The first attribute or a forward sequence
            *   \tparam A2 The second attribute or nothing
            *   \tparam A3 The third attribute or nothing
            *   \tparam A4 The fourth attribute or nothing
            *   \tparam A5 The fifth attribute or nothing
            *   \tparam A6 The sixth attribute or nothing
            *   \tparam A7 The seventh attribute or nothing
            *   \tparam A8 The eighth attribute or nothing
            *   \tparam A9 The ninth attribute or nothing
            *   \tparam A10 The tenth attribute or nothing
            *
            */
            template <typename A1 = boost::mpl::na, typename A2 = boost::mpl::na,
                      typename A3 = boost::mpl::na, typename A4 = boost::mpl::na,
                      typename A5 = boost::mpl::na, typename A6 = boost::mpl::na,
                      typename A7 = boost::mpl::na, typename A8 = boost::mpl::na,
                      typename A9 = boost::mpl::na, typename A10 = boost::mpl::na>
            struct AttributeSeq : famouso::mw::attributes::detail::SetProvider<
                                     A1, A2, A3, A4, A5, A6, A7, A8, A9, A10
                                  >::attrSet {
                typedef AttributeSeq    type;
            };

        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso

#endif // __ATTRIBUTESEQ_H_DD54D24E66A1EA__

