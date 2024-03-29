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

#ifndef _Attribute_Set_h_
#define _Attribute_Set_h_

#include <stdint.h>
#include <string.h>

#include "boost/mpl/is_sequence.hpp"
#include "boost/mpl/vector.hpp"
#include "boost/mpl/find_if.hpp"
#include "boost/mpl/void.hpp"
#include "boost/mpl/placeholders.hpp"
#include "boost/mpl/deref.hpp"
#include "boost/mpl/eval_if.hpp"

#include "assert/staticerror.h"

#include "object/PlacementNew.h"

#include "mw/attributes/detail/AttributeSetImpl.h"
#include "mw/attributes/detail/find.h"
#include "mw/attributes/detail/AttributeSize.h"
#include "mw/attributes/detail/Duplicates.h"
#include "mw/attributes/detail/AttributeCompileErrors.h"

#include "mw/attributes/access/Attribute_RT.h"
#include "mw/attributes/access/AttributeSet_RT.h"
#include "mw/attributes/access/AttributeSetHeader_RT.h"

#include "mw/attributes/Null.h"
#include "mw/attributes/type_traits/is_same_base_type.h"

namespace famouso {
    namespace mw {
        namespace attributes {

            /*!
             * \brief Represents a sequence of %attributes in binary representation
             *
             * \tparam AttrSeq The sequence of %attributes, the sequence elements
             *  should be derived from Attribute, see this struct's description
             *  for requirements concerning an attribute type
             */
            template <typename AttrSeq = boost::mpl::vector<> >
            struct AttributeSet : public famouso::mw::attributes::access::AttributeSet_RT {
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

                    // The content size (the bytes needed for the attribute data) is determined
                    //  by the implementation struct
                    static const uint16_t contentSize = impl::size;

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
                    // True, if the set header will be written extended, false if the
                    //  header fits one byte
                    static const bool extension = (contentSize > 0x7F);

                    // Assert that the sequence size fits the format bounds
                    typedef typename boost::mpl::eval_if_c<
                                                  extension,
                                                  detail::ExtendedSequenceBoundError<contentSize>,
                                                  detail::UnextendedSequenceBoundError<contentSize>
                                                 >::type assertDummy;

                    // The size of the header is 1 byte if it is not extended and 2 bytes
                    //  if it is extended
                    static const uint8_t headerSize = (extension ? 2 : 1);

                public:
                    /*!
                     * \brief The size of the complete attribute sequence in binary representation
                     *
                     * This includes the sequence header and every single attribute contained in
                     *  the sequence.
                     */
                    static const uint16_t overallSize = headerSize + contentSize;

                private:
                    /*!
                     * \brief The member array containing the complete attribute sequence
                     * 	including its header
                     */
                    uint8_t data[overallSize];

                public:
                    /*!
                     * \brief Constructor creating the binary representation of the attribute
                     *  sequence into its member array.
                     */
                    AttributeSet() {
                        // The header always starts at index 0 (In this case we do not have
                        //  to consider the offset since it is always 0 at this point)
                        writeSize(contentSize, extension);

                        // The construction of the attribute binary data is done by the
                        //  wrapped implementation
                        impl::construct(&data[headerSize]);
                    }

                    /*!
                     * \brief Searches for the attribute given as a template argument in
                     *  the compile time representation of this attribute set.
                     *
                     * The found attribute can be accessed by the typedef "type". If the
                     *  given attribute could not be found, type will be the Null attribute.
                     *
                     * \tparam Attr The attribute type which should be searched for
                     */
                    template <typename Attr>
                    struct find_ct {
                        private:
                            // The predicate for finding the specified attribute
                            typedef famouso::mw::attributes::type_traits::is_same_base_type<
                                                                           boost::mpl::_1,
                                                                           Attr
                                                                          > pred;

                            // Search the attribute in the underlying sequence
                            typedef typename boost::mpl::find_if<
                                                          sequence,
                                                          pred
                                                         >::type found;

                            // Helper struct to map void_ to Null
                            template <typename Found, bool dummy>
                            struct helper {
                                    typedef Found type;
                            };
                            template <bool dummy>
                            struct helper<boost::mpl::void_, dummy> {
                                    typedef Null type;
                            };

                        public:
                            typedef typename helper<
                                              typename boost::mpl::deref<found>::type,
                                              true
                                             >::type type;
                    };

            };

        } // end namespace attributes
    } // end namespace mw
} // end namespace famouso


#endif // _Attribute_Set_
