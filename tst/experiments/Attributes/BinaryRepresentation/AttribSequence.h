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

#ifndef _AttrSequence_
#define _AttrSequence_

#include <stdint.h>
#include <string.h>

#include "boost/mpl/assert.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/begin.hpp"
#include "boost/mpl/end.hpp"
#include "boost/mpl/next.hpp"
#include "boost/mpl/deref.hpp"
#include "boost/mpl/size.hpp"

#include "util/endianness.h"

#include "Utils.h"

#include "find_.h"

/*!
 * /brief Represents an attribute sequence header which simply
 *  provides a binary representation for the sequence's size,
 *  i.e. the number of elements (attributes in this case) which
 *  are contained.
 *
 * The binary representation basically consists of one byte. The
 *  highest bit of this byte signals if another byte will follow.
 *  If set to 0 the remaining 7 bits represent the actual size. That
 *  is, in that case sizes up to 127 are possible.
 *  If set to 1 the following 15 bits (including the 8 bits of the
 *  next byte) represent the size. In this case sizes up to 32.768
 *  are possible.
 *
 * /tparam seqSize The size which should be represented
 */
template <uint16_t seqSize>
struct SequenceHeaderWriter {
	private:
		typedef SequenceHeaderWriter type;

		enum {
			// True, if the list header will be written extended, false if the
			//  header fits one byte
			extension = (seqSize > 0x7F)
		};

	public:
		enum {
			// The size of the header is 1 if it is not extended and 2 if it
			//  is extended
			size      = (extension ? 2 : 1)
		};

	private:
		// The member array containing the binary represented list header
		// (the size is decided by the given sequence size)
		uint8_t data[size];

	public:
		/*!
		 * Creates the binary representation of the size given as the type
		 *  parameter in to the member array.
		 */
		SequenceHeaderWriter() {
			// Depending on whether the sequence header is extended either 1
			//  or 2 two bytes must be written accordingly
			if (extension) {
				// In case of an extension, assert that the sequence size does not
				//  exceed the bounds of 15 bits (0x7FFF)
				BOOST_MPL_ASSERT_MSG(((seqSize & 0x7FFF) == seqSize),
															 sequence_length_exceeds_extended_format_bounds,
															 (boost::mpl::int_<seqSize>));

				// Convert the length to network byte order
				const uint16_t       tmpSize = htons(seqSize);
				const uint8_t* const tmpPtr  = reinterpret_cast<const uint8_t* const>(&tmpSize);

				// Copy it bytewise into the member array
				// Set the extension bit
				data[0] = (tmpPtr[0] | 0x80);
				data[1] = tmpPtr[1];
			} else {
				// In case of no extension, assert that the sequence size does not
				//  exceed the bounds of 7 bits (0x7F)

				// TODO: Same about this assertions here (see the attribute header
				//  writer for an explanation)

				// BOOST_MPL_ASSERT_MSG(((seqSize & 0x7F) == seqSize),
				//		             sequence_length_exceeds_unextended_format_bounds,
				//		             (boost::mpl::int_<seqSize>));

				// Write the lower 7 bits of the sequence size into the first and
				//  only byte (The extension flag assures that the given sequence
				//  size fits 7 bits)
				data[0] = (seqSize & 0x7F);
			}
		}
};

/*!
 * /brief Represents a sequence of attributes in binary representation
 *
 *
 *
 * /tparam AttrSeq The sequence of attributes, the sequence elements
 *  should be derived from EmptyAttribute, see this struct's description
 *  for requirements concerning an attribute type
 *
 * /tparam Iter The iterator pointing to the current element of the
 *  attribute sequence, should always be left to the default value from
 *  outside
 */
template <typename AttrSeq, typename Iter = typename boost::mpl::begin<AttrSeq>::type>
struct AttribSequence {
	private:
		// The current attribute type (determined by the given iterator type)
		typedef boost::mpl::deref<Iter> curAttr;
		// The next sequence element type
		typedef boost::mpl::next<Iter>  iterNext;

		// The sequence header structure type always instantiated with the size of the
		//  complete attribute sequence ("always" means that this type is also instantiated
		//  for the recursively instantiated remaining sequences, where (isFirst == false),
		//  nevertheless it will not really be used in this case)
		typedef SequenceHeaderWriter<boost::mpl::size<AttrSeq>::type::value> seqHeader;

		//typedef SequenceHeaderWriter<0x7F> seqHeader;
		//typedef SequenceHeaderWriter<0xFF> seqHeader;
		//typedef SequenceHeaderWriter<0x1FFF> seqHeader;

		enum {
			// Determines whether the iterator points to the first attribute of the
			//  the sequence (If this is true, the sequence header will be written
			//  additionally)
			isFirst = is_same<Iter, typename boost::mpl::begin<AttrSeq>::type>::value,

			// The offset determines where the actual attribute data starts in the
			//  member array, this depends on whether we currently handle the first
			//  attribute in list or not; In the former case the offset is identical
			//  to the size of the sequence header since it will be written before the
			//  the first attribute of the sequence, in the latter case the offset is
			//  simply 0 since there will be no sequence header written
			offset  = (isFirst ? seqHeader::size : 0)
		};

	public:
		/*!
		 * /brief This type
		 */
		typedef AttribSequence type;

		enum {
			/*!
			 * /brief The size of the complete attribute sequence in binary representation
			 *
			 * This includes the sequence header and every single attribute contained in
			 *  the sequence.
			 */
			overallSize = // If the current attribute is the first, the sequence header will be included
										(isFirst ? seqHeader::size : 0) +

										// In any case (first or not) the current attribute is included (the size
										//  calculator struct calculates the overall size of an attribute including
										//  the header)
										SizeCalculator<typename curAttr::type>::value +

										// Recursively instantiate the attribute sequence with its iterator
										//  pointing to next attribute in the sequence (the past-end case
										//  is modeled by a specialization of the attribute sequence struct
										//  which has an overall size of 0)
										AttribSequence<AttrSeq, typename iterNext::type>::overallSize
		};

		/*!
		 * /brief The member array containing the complete attribute sequence
		 *
		 * This should only be accessed with no specific iterator (i.e. the iterator
		 *  pointing to first sequence element) given in the type argument list.
		 *  Otherwise it would contain just the attribute sequence starting from the
		 *  given iterator and no sequence header.
		 */
		uint8_t data[overallSize];

		/*!
		 * /brief Constructor creating the binary representation of the attribute sequence
		 *  into its member array.
		 *
		 * Since this class is a recursive structure, the constructor only creates the
		 * 	binary representation of the current single attribute. The remaining attributes
		 *  (the current sequence position is given by the iterator type argument
		 *  above) are written by another recursively instantiated class of this
		 *  template class. If the current attribute is the first one in the sequence the
		 *  list header (i.e. the attribute count) is also written.
		 */
		AttribSequence() {
			// If the iterator points to the first attribute in the sequence, write the
			//  attribute sequence header
			if (isFirst) {
				// The header always starts at index 0
				new (&data[0]) seqHeader;
			}

			// The next two steps must necessarily be performed in the given order since the
			//  attribute will overwrite the bytes which it considers to be assigned to its
			//  value and the attribute header writer will allow for this (it will assign its
			//  data to the array or use an OR operation depending on the binary structure
			//  implied by the attribute, see the description of the CaseSelector and the
			//  the ValueOffsetCalculator for an explanation of the possible binary structures
			//  of an attribute)

			// TODO: This is the main construction site when the attribute class itself contains
			//  the header...

			// Construct the current attribute (This is done by instantiating the current
			//  attribute type into the member array at the correct offset, the correct resulting
			//  offset is calculated by the ValueOffsetCalculator since the attribute itself should
			//  not be aware of any offsets in a resulting structure, just manage and write its value)
			new (&data[offset]) typename curAttr::type;

			// Let the rest of the sequence construct its attributes into the rest of the array
			//  (This is done by recursively instantiate another one of this template struct
			//   incrementing the iterator to the next attribute of the sequence, the past-end
			//   case is modeled separately by a specialization of this template struct)
			new (&data[offset + SizeCalculator<typename curAttr::type>::value]) AttribSequence<AttrSeq, typename iterNext::type>;
		}

	private:
		template <typename S, typename I = typename boost::mpl::begin<S>::type>
		struct Printer {
			typedef boost::mpl::deref<I> attrType;

			static void print(const uint8_t* data) {
				::logging::log::emit() << "[ID=" << static_cast<uint16_t>(attrType::type::id) << "|";

				typename attrType::type* a = find::template find<typename attrType::type>(data);

				if (a == NULL) {
					::logging::log::emit() << "|NIL]";
				} else {
					::logging::log::emit() << "|Val=" << static_cast<typename attrType::type::valueType>(a->get()) << ']';
				}

				Printer<S, typename boost::mpl::next<I>::type>::print(data);
			}
		};

		template <typename S>
		struct Printer<S, typename boost::mpl::end<S>::type> {
				static void print(const uint8_t* data) {
					::logging::log::emit() << ::logging::log::endl;
				}
		};

	public:

		void printRT() {
			Printer<AttrSeq>::print(data);
		}

		static void print() {
			if (isFirst) {
				::logging::log::emit() << "[Hdr]";
			}

			::logging::log::emit() << " [A|";
			::logging::log::emit() << (curAttr::type::isSystem ? "S" : "N");
			::logging::log::emit() << "|ID:" << (uint16_t) curAttr::type::id;
			::logging::log::emit() << "|VAL:" << (uint64_t) curAttr::type::value << ']';

			AttribSequence<AttrSeq, typename iterNext::type>::print();
		}
};
template <typename AttrSeq>
struct AttribSequence<AttrSeq, typename boost::mpl::end<AttrSeq>::type> {
	enum {
		overallSize = 0
	};

	static void print() {
		::logging::log::emit() << ::logging::log::endl;
	}
};


#endif // _AttrSequence_
