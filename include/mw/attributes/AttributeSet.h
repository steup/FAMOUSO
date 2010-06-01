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

#include "boost/mpl/begin.hpp"
#include "boost/mpl/end.hpp"
#include "boost/mpl/next.hpp"
#include "boost/mpl/deref.hpp"
#include "boost/mpl/size.hpp"
#include "boost/mpl/is_sequence.hpp"
#include "boost/type_traits/is_same.hpp"

#include "object/PlacementNew.h"

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
             *
             * \tparam Iter The iterator pointing to the current element of the
             *  attribute sequence, should always be left to the default value from
             *  outside
             */
            template <typename AttrSeq, typename Iter = typename boost::mpl::begin<AttrSeq>::type>
            struct AttributeSet {
                private:
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

                    // The current attribute type (determined by the given iterator type)
                    typedef boost::mpl::deref<Iter> curAttr;
                    // The next sequence element type
                    typedef boost::mpl::next<Iter> iterNext;

                    // Determines whether the iterator points to the first attribute of the
                    //  the sequence (If this is true, the sequence header will be written
                    //  additionally)
                    static const bool isFirst = boost::is_same<
                                                        Iter,
                                                        typename boost::mpl::begin<AttrSeq>::type
                                                       >::value;

                public:
                    /*!
                     * \brief This type
                     */
                    typedef AttributeSet type;

                    /**
                     * \brief The sequence that was given to create this set
                     */
                    typedef AttrSeq      sequence;

                    /*
                     * \brief The size of this set in bytes, this does not include the set header only
                     *  the representation of all attributes is considered.
                     */
                    static const uint16_t setSize = detail::AttributeSize<typename curAttr::type>::value +
                                                    AttributeSet<AttrSeq, typename iterNext::type>::setSize;

                    static const uint16_t count = boost::mpl::size<AttrSeq>::value;

                private:
                    // The sequence header structure type always instantiated with the size of the
                    //  complete attribute sequence ("always" means that this type is also instantiated
                    //  for the recursively instantiated remaining sequences, where (isFirst == false),
                    //  nevertheless it will not really be used in this case)
                    typedef detail::AttributeSetHeader<setSize> setHeader;

                    // The offset determines where the actual attribute data starts in the
                    //  member array, this depends on whether we currently handle the first
                    //  attribute in list or not; In the former case the offset is identical
                    //  to the size of the sequence header since it will be written before the
                    //  the first attribute of the sequence, in the latter case the offset is
                    //  simply 0 since there will be no sequence header written
                    static const uint8_t offset = (isFirst ? setHeader::size : 0);

                public:
                    /*!
                     * \brief The size of the complete attribute sequence in binary representation
                     *
                     * This includes the sequence header and every single attribute contained in
                     *  the sequence.
                     */
                    static const uint16_t overallSize =
                            // If the current attribute is the first, the sequence header will
                            //  be included
                            (isFirst ? setHeader::size : 0) +

                            // The size without the header has already been calculated above
                            setSize;

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
                        // If the iterator points to the first attribute in the sequence, write
                        //  the attribute sequence header
                        if (isFirst) {
                            // The header always starts at index 0 (In this case we do not have
                            //  to consider the offset since it is always 0 at this point)
                            new (&data[0]) setHeader;
                        }

                        // The next two steps must necessarily be performed in the given order
                        //  since the attribute will overwrite the bytes which it considers to
                        //  be assigned to its value and the attribute header writer will allow
                        //  for this (it will assign its data to the array or use an OR operation
                        //  depending on the binary structure implied by the attribute, see the
                        //  description of the CaseSelector and the the ValueOffsetCalculator
                        //  for an explanation of the possible binary structures of an attribute)

                        // Construct the current attribute (This is done by instantiating the
                        //  current attribute type into the member array at the correct offset,
                        //  the correct resulting offset is calculated by the ValueOffsetCalculator
                        //  since the attribute itself should not be aware of any offsets in a
                        //  resulting structure, just manage and write its value)
                        new (&data[offset]) typename curAttr::type;

                        // Let the rest of the sequence construct its attributes into the rest of
                        //  the array (This is done by recursively instantiate another one of this
                        //  template struct incrementing the iterator to the next attribute of the
                        //  sequence, the past-end case is modeled separately by a specialization
                        //  of this template struct)
                        new (&data[offset + detail::AttributeSize<typename curAttr::type>::value])
                                AttributeSet<AttrSeq, typename iterNext::type> ;
                    }

                public:
                    template <typename Attr>
                    Attr* find() {
                        // Check if the attribute is contained in the static sequence at all
                        if (!detail::IsContained<Attr, AttrSeq>::result::value) {
                            return (reinterpret_cast<Attr*>(NULL));
                        }

                        return (famouso::mw::attributes::detail::find<Attr>(data));
                    }

                    template <typename Attr>
                    const Attr* find() const {
                        // Check if the attribute is contained in the static sequence at all
                        if (!detail::IsContained<Attr, AttrSeq>::result::value) {
                            return (reinterpret_cast<Attr*>(NULL));
                        }

                        return (famouso::mw::attributes::detail::find<Attr>(data));
                    }
            };

            template <typename AttrSeq>
            struct AttributeSet<AttrSeq, typename boost::mpl::end<AttrSeq>::type> {
                    BOOST_MPL_ASSERT_MSG((boost::mpl::is_sequence<AttrSeq>::value),
                                         no_forward_sequence_given,
                                         (AttrSeq));

                    static const uint16_t setSize = 0;

                    static const uint16_t overallSize = 0;

                    typedef AttrSeq sequence;

                    typedef AttributeSet type;

                    template <typename Attr>
                    Attr* find() {
                        return (reinterpret_cast<Attr*> (NULL));
                    }

                    template <typename Attr>
                    const Attr* find() const {
                        return (reinterpret_cast<Attr*> (NULL));
                    }
            };

        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso


#endif // _Attribute_Set_
