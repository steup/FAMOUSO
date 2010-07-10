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

#include "assert/staticerror.h"

#include "object/PlacementNew.h"

#include "mw/attributes/detail/AttributeSetImpl.h"
#include "mw/attributes/detail/find.h"
#include "mw/attributes/detail/FindStatic.h"
#include "mw/attributes/detail/AttributeSetHeader.h"
#include "mw/attributes/detail/AttributeSize.h"
#include "mw/attributes/detail/Duplicates.h"

// TODO: See the TODO below
#include "mw/attributes/TTL.h"

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
                    FAMOUSO_STATIC_ASSERT_ERROR(
                        (boost::mpl::is_sequence<AttrSeq>::value),
                        no_forward_sequence_given,
                        (AttrSeq));

                    // Duplicate-Tester for the given attribute sequence
                    typedef detail::Duplicates<AttrSeq> duplicateTester;

                    // Assert that the sequence does not contain duplicates, if it does print out
                    //  the first attribute in the sequence for which a duplicate could be found
                    //  and the whole sequence, too
                    FAMOUSO_STATIC_ASSERT_ERROR(
                        (!duplicateTester::result),
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
                    /*!
                     * \brief Searches for the attribute given as a template argument in the
                     *  binary representation of this attribute set and returns it
                     *
                     * The target attribute is searched for using the isSystem property and
                     *  the ID of the given attribute type. The returned instance can then
                     *  be used to access the value.
                     * If the given attribute could not be found in the set, NULL is returned.
                     *
                     * \tparam Attr The attribute type which should be searched for
                     *
                     * \return An instance of Attr or NULL if the attribute could not be found
                     */
                    template <typename Attr>
                    Attr* find() {
                        return (famouso::mw::attributes::detail::find<Attr>(&data[0]));
                    }

                    /*!
                     * \brief Searches for the attribute given as a template argument in the
                     *  binary representation of this attribute set and returns it.
                     *
                     * The target attribute is searched for using the isSystem property and
                     *  the ID of the given attribute type. The returned instance can then
                     *  be used to access the value.
                     * If the given attribute could not be found in the set, NULL is returned.
                     *
                     * \tparam Attr The attribute type which should be searched for
                     *
                     * \return A constant instance of Attr or NULL if the attribute could
                     *  not be found
                     */
                    template <typename Attr>
                    const Attr* find() const {
                        return (famouso::mw::attributes::detail::find<Attr>(&data[0]));
                    }

                    /*!
                     * \brief Returns the number of bytes used for the encoded attributes in
                     *  this set.
                     *
                     * This excludes the number of bytes used for the set header itself.
                     *
                     * \return The number of bytes used for the attributes of this set
                     */
                    uint16_t getSize() const {
                        return (reinterpret_cast<const setHeader* const>(&data[0])->get());
                    }

                    // TODO: When there is a way for API accessor types implemented, remove this
                    typedef TTL<0> dummyAttr;

                    uint16_t getCount() const {
                        const uint8_t* ptr = data;

                        uint16_t result = 0;

                        // TODO: This is a copy-paste-version of the find method, consider implementing
                        //  an iterator class

                        // The number of bytes needed by the attributes
                        //  contained in the given sequence
                        uint16_t seqLen;

                        {
                            // TODO: Think about a special API accessor type instead of using
                            //  the template type with zero manually
                            const detail::AttributeSetHeader<0>* const setHeader =
                                    reinterpret_cast<const detail::AttributeSetHeader<0>* const>(&ptr[0]);

                            seqLen = setHeader->get();

                            ptr += (setHeader->isExtended() ? 2 : 1);
                        }

                        // The pointer were the given sequence ends
                        const uint8_t* const targetPtr = ptr + seqLen;

                        while (ptr < targetPtr) {
                            ++result;

                            // We let the attribute class determine its overall size to skip it
                            ptr += reinterpret_cast<const dummyAttr* const>(&ptr[0])->size();
                        }

                        // If we iterated the complete attribute sequence the intended attribute
                        //  could not be found and NULL is returned
                        return (result);
                    }

                    // TODO: Implement methods for manipulation of the attribute set at runtime
            };

        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso


#endif // _Attribute_Set_
