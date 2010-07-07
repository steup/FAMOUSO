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

#ifndef _Attribute_Set_
#define _Attribute_Set_

#include <stdint.h>

#include "boost/mpl/is_sequence.hpp"
#include "boost/mpl/vector.hpp"

#include "object/PlacementNew.h"

#include "mw/attributes/detail/AttributeSetImpl.h"
#include "mw/attributes/detail/find.h"
#include "mw/attributes/detail/FindStatic.h"
#include "mw/attributes/detail/AttributeSetHeader.h"
#include "mw/attributes/detail/AttributeSize.h"
#include "mw/attributes/detail/Duplicates.h"

namespace famouso {
    namespace mw {
        namespace attributes {

            /*!
             * \brief Represents a sequence of attributes in binary representation
             *
             * \tparam AttrSeq The sequence of attributes, the sequence elements
             *  should be derived from Attribute, see this struct's description
             *  for requirements concerning an attribute type
             */
            template <typename AttrSeq = boost::mpl::vector<> >
            struct AttributeSet {
                private:
                    // Assert that a forward sequence is given
                    BOOST_MPL_ASSERT_MSG((boost::mpl::is_sequence<AttrSeq>::value),
                                         no_forward_sequence_given,
                                         (AttrSeq));

                    // Duplicate-Tester for the given attribute sequence
                    typedef detail::Duplicates<AttrSeq> duplicateTester;

                    // Assert that the sequence does not contain duplicates, if it does print out
                    //  the first attribute in the sequence for which a duplicate could be found
                    //  and the whole sequence, too
                    BOOST_MPL_ASSERT_MSG((!duplicateTester::result),
                                         duplicate_attribute_detected_in_sequence,
                                         (typename duplicateTester::duplicateAttribute, AttrSeq));

                    // The implementation struct of the actual attribute set
                    typedef detail::AttributeSetImpl<AttrSeq> impl;

                public:
                    /*!
                     * \brief This type
                     */
                    typedef AttributeSet type;

                    /**
                     * \brief The sequence that was given to create this set
                     */
                    typedef AttrSeq sequence;

                private:
                    // The sequence header structure type is instantiated with the size of the
                    //  complete attribute sequence
                    typedef detail::AttributeSetHeader<impl::size> setHeader;

                public:
                    /*!
                     * \brief The size of the complete attribute sequence in binary representation
                     *
                     * This includes the sequence header and every single attribute contained in
                     *  the sequence.
                     */
                    static const uint16_t overallSize = setHeader::size + impl::size;

                private:
                    /*!
                     * \brief The member array containing the complete attribute sequence
                     *
                     * This should only be accessed with no specific iterator (i.e. the iterator
                     *  pointing to first sequence element) given in the type argument list.
                     *  Otherwise it would contain just the attribute sequence starting from the
                     *  given iterator and no sequence header.
                     */
                    uint8_t data[overallSize];

                public:
                    /*!
                     * \brief Constructor creating the binary representation of the attribute
                     *  sequence into its member array.
                     *
                     * Since this class is a recursive structure, the constructor only creates
                     *  the binary representation of the current single attribute. The remaining
                     *  attributes (the current sequence position is given by the iterator type
                     *  argument above) are written by another recursively instantiated class of
                     *  this template class. If the current attribute is the first one in the
                     *  sequence the list header (i.e. the attribute count) is also written.
                     */
                    AttributeSet() {
                        // The header always starts at index 0 (In this case we do not have
                        //  to consider the offset since it is always 0 at this point)
                        new (&data[0]) setHeader;

                        // The construction of the attribute binary data is done by the
                        //  wrapped implementation
                        impl::construct(&data[setHeader::size]);
                    }

                public:
                    template <typename Attr>
                    Attr* find() {
                        return (famouso::mw::attributes::detail::find<Attr>(data));
                    }

                    template <typename Attr>
                    const Attr* find() const {
                        return (famouso::mw::attributes::detail::find<Attr>(data));
                    }
            };

        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso


#endif // _Attribute_Set_
