/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 *               2009-2010 Michael Kriese <kriese@cs.uni-magdeburg.de>
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
#ifndef _ComparableAttributeSet_h_
#define _ComparableAttributeSet_h_

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/mpl/list.hpp>
#include <boost/asio/buffer.hpp>
#include "mw/attributes/AttributeSet.h"
#include "mw/attributes/filter/greater_than_or_equal_to.h"
#include "mw/attributes/filter/less_than_or_equal_to.h"
#include "mw/nl/awds/AWDS_Packet.h"
#include "mw/nl/awds/logging.h"

#include "mw/attributes/access/Attribute_RT.h"
#include "mw/attributes/access/AttributeSet_RT.h"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {
                namespace detail {
                    /*!
                     * \brief Helper structs for printing the operator of a given
                     *  comparator.
                     */
                    template< typename Comparator >
                    struct op_printer {
                            static const char* op() {
                                return (" ? ");
                            }
                    };
                    template< >
                    struct op_printer<famouso::mw::attributes::filter::less_than_or_equal_to> {
                            static const char* op() {
                                return (" <= ");
                            }
                    };
                    template< >
                    struct op_printer<famouso::mw::attributes::filter::greater_than_or_equal_to> {
                            static const char* op() {
                                return (" >= ");
                            }
                    };
                } /* namespace detail */

                /* \brief A class for holding attributes of publisher, subscriber or network.
                 *
                 * \tparam AttrSet An AttributesSet containing a list of attributes.
                 */
                template< class AttrSet = famouso::mw::attributes::AttributeSet<> >
                class ComparableAttributeSet: boost::noncopyable {

                    public:
                        /** The attributes sequence contained in AttrSet. */
                        typedef typename AttrSet::sequence AttrSeq;

                    private:
                        /** An array for storing the attributes data. */
                        typedef boost::shared_array<uint8_t> pData;

                        pData data; /**< The data where the attributes are stored. */

                        /**
                         * An iterator for printing and checking a list of attributes.
                         *
                         * \tparam Seq The attributes sequence.
                         * \tparam Itr An iterator to Seq.
                         */
                        template< class Seq, class Itr = typename boost::mpl::begin<Seq>::type >
                        class AttributeIterator {

                                /** The actual attribute type. */
                                typedef typename boost::mpl::deref<Itr>::type attrib;

                                /** The actual attribute comparator. */
                                typedef typename attrib::comparator cmp;

                                /** The type of the last attribute in the list Seq. */
                                typedef typename boost::mpl::end<Seq>::type end;

                                /**
                                 * Print an attributes value or "nd" if the attribute is not defined.
                                 */
                                static void echo(::logging::loggingReturnType &out, attrib *a) {
                                    if (a)
                                        out << (intmax_t) a->getValue();
                                    else
                                        out << "nd";
                                }

                                /** \brief Checks if two attributes are matching.
                                 *
                                 * The first attribute should be the actual network attribute and the second attribute can be the
                                 * publisher or subscriber defined attribute. The network attributes always have to be defined.
                                 * The publisher or subscriber defined attributes can be not defined (then the match is always true
                                 * because we don't have to comply with it). The actual comparator is defined by the attribute itself.
                                 *
                                 * \param a The first attribute.
                                 * \param b The second attribute.
                                 * \return true if the attributes match, otherwise false.
                                 */
                                static bool match(const attrib *a, const attrib *b) {
                                    // if attrib b is empty check is always true
                                    if (!b)
                                        return true;

                                    // if attrib b is not empty, attrib a has to be not empty too, so check if it matches
                                    if (a && cmp::apply_runtime(a->getValue(), b->getValue()))
                                        return true;

                                    // attributes doesn't match
                                    return false;
                                }

                            public:

                                /**
                                 * Iterates over the attributes and check them against the two given lists.
                                 * The first list should be the actual network attributes and the second list can be the
                                 * publisher or subscriber defined attributes.
                                 *
                                 * \param a The first attribute list.
                                 * \param b The second attribute list.
                                 * \return true if all attributes match, otherwise false.
                                 */
                                static bool match(AttrSet &a, AttrSet &b) {
                                    // find attributes
                                    attrib *l = a.template find_rt<attrib> ();
                                    attrib *r = b.template find_rt<attrib> ();

                                    ::logging::loggingReturnType &out = log::emit<ATTR>();

                                    out << "matching " << (int) attrib::id << ": ";
                                    echo(out, l);
                                    out << detail::op_printer<cmp>::op();
                                    echo(out, r);
                                    out << log::endl;

                                    // match attribute
                                    if (!match(l, r)) {
                                        log::emit<ATTR>() << "unsuccessfull" << log::endl;
                                        return false; // if attribs not match
                                    }

                                    // Check the next attribute in list
                                    return AttributeIterator<Seq, typename boost::mpl::next<Itr>::type>::match(a, b);
                                }

                                /**
                                 * Prints all attributes to the output stream.
                                 *
                                 * \param out The output stream.
                                 * \param a The attribute list.
                                 */
                                static void print(::logging::loggingReturnType &out, AttrSet &a) {
                                    // find the attribute
                                    attrib *attr = a.template find_rt<attrib> ();
                                    if (attr) {
                                        // found, so print name and value
                                        out << " [" << (int) attrib::id << "]={" << (intmax_t) attr->getValue() << "}";
                                    }

                                    // print next attribute in list
                                    AttributeIterator<Seq, typename boost::mpl::next<Itr>::type>::print(out, a);
                                }
                        };

                        /**
                         * Specialized template for end of recursion.
                         *
                         * \tparam Seq The attribute sequence.
                         */
                        template< class Seq >
                        class AttributeIterator<Seq, typename boost::mpl::end<Seq>::type> {
                            public:

                                static bool match(const AttrSet &a, const AttrSet &b) {
                                    log::emit<ATTR>() << "successfull" << log::endl;
                                    return true;
                                }

                                static void print(::logging::loggingReturnType &out, const AttrSet &a) {
                                }
                        };

                        /* \brief Copy the data to this instance.
                         *
                         * \param d the data pointer
                         * \param s the size of the data
                         */
                        void set(uint8_t *d, uint16_t s) {
                            static uint8_t p[1] = { 0 };

                            if (s <= 0) {
                                s = 1;
                                d = p;
                            }
                            data = pData(new uint8_t[s]);
                            std::memcpy(data.get(), d, s);

                        }

                        /** \brief Compute the size of the attributes data.
                         *
                         *  \param data A pointer to the attribute set data.
                         */
                        static uint16_t size(uint8_t* data) {

                            if (!data) // just to be a little secure.
                                return 0;

                            typedef famouso::mw::attributes::access::AttributeSet_RT *AS;

                            const AS set_rt = reinterpret_cast<const AS> (&data[0]);

                            return set_rt->length();
                        }

                        /** \brief Contructor to init the set with no attributes.
                         *
                         * Init the data with one byte otherwise we get SIGSEGV when accessing at getSeq().
                         * The famouso::mw::attributes::AttributeSet is looking at the first byte of the data.
                         */
                        ComparableAttributeSet() {
                            set(NULL, 0);
                        }

                        /** \brief Return a reference to the attributes set.
                         *
                         * \return A reference to the attributes set.
                         */
                        AttrSet &getSet() {
                            return *reinterpret_cast<AttrSet*> (data.get());
                        }

                    public:
                        /** \copydoc Attributes */
                        typedef boost::shared_ptr<ComparableAttributeSet> type;

                        /**
                         * \brief Copy the attributes from the packet to this attribute instance.
                         *
                         * \param p The packet to copy from.
                         */
                        void set(awds::AWDS_Packet &p) {
                            // if this is not an attributes packet
                            if (p.header.type != AWDS_Packet::constants::packet_type::attributes)
                                return;

                            set(p.data, size(p.data));
                        }

                        /*! \brief Checks wether all attributes are matching
                         *
                         *  The first attribute list should be the actual network attributes and the second attribute can be the
                         *  publisher or subscriber defined attributes. The network attributes always have to be defined.
                         *  The publisher or subscriber defined attributes can be not defined (then the match is always true
                         *  because we don't have to comply with it). The actual comparator is defined by the attribute itself.
                         *
                         *  \param a The first attributes list
                         *  \param b The second attributes list.
                         *  \return true if all attributes of the first list matches the second list of attributes, otherwise false.
                         */
                        static bool match(const type & a, const type & b) {
                            return AttributeIterator<AttrSeq>::match(a->getSet(), b->getSet());
                        }

                        /*! \brief print the attributes to the given stream.
                         *
                         *  \param out the output stream to print to.
                         */
                        void print(::logging::loggingReturnType &out) {
                            out << ::logging::log::dec;
                            AttributeIterator<AttrSeq>::print(out, getSet());
                        }

                        /** \brief Creates a copy of the attributes set.
                         *
                         *  \return The copied attributes set.
                         */
                        type clone() {
                            type res = create(getSet());
                            return res;
                        }

                        /** \brief Find an attribute in the set.
                         *
                         *  \tparam Attrib The attribute to find.
                         *  \return A pointer to the Attribute if found, NULL otherwise.
                         */
                        template< class Attrib >
                        Attrib *find() {
                            return getSet().template find_rt<Attrib> ();
                        }

                        /** \brief Converts this set to a buffer to be useable by boost io.
                         *
                         * \return A boost buffer to use for io.
                         */
                        operator boost::asio::const_buffer() {
                            uint8_t *d = data.get();
                            return boost::asio::buffer(d, size(d));
                        }


                        /** \brief Returns the overall runtime size of the attribute set.
                         *
                         * \return The overall runtime size of the attribute set.
                         */
                        uint16_t size(){
                            return size(data.get());
                        }

                        /*! \brief Creates an empty attributes instance.
                         *
                         *  \return An instance of attributes.
                         */
                        static type create() {
                            type res = type(new ComparableAttributeSet());
                            return res;
                        }

                        /*! \brief Creates an new attributes instance.
                         *
                         *  \param p An AWDS_Packet to load attributes from.
                         *  \return An instance of attributes.
                         */
                        static type create(AWDS_Packet &p) {
                            type res = create();
                            res->set(p);
                            return res;
                        }

                        /*! \brief Creates a new attributes instance.
                         *
                         *  The attributes data of the given set ist copied.
                         *
                         *  \param p An attribute set to load attributes from.
                         *  \return The new attributes set.
                         *  \tparam AttrSeq2 A boost mpl list of Attributes.
                         */
                        template< class AttrSeq2 >
                        static type create(famouso::mw::attributes::AttributeSet<AttrSeq2> &p) {
                            type res = create();
                            uint8_t *d = reinterpret_cast<uint8_t*> (&p);
                            res->set(d, size(d));
                            return res;
                        }

                        /*! \brief Creates a new attributes instance.
                         *
                         *  The attributes data of the given set ist copied.
                         *
                         *  \param d A pointer to an attribute set to copy attributes from.
                         *  \return The new attributes set.
                         */
                        static type create(uint8_t *d) {
                            type res = create();
                            if (d)
                                res->set(d, size(d));
                            return res;
                        }
                };

            } /* namespace awds */
        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */

namespace logging {
    using famouso::mw::nl::awds::ComparableAttributeSet;

    /*! \brief Print the attributes to the given stream.
     *
     *
     *  \param out the output stream to print to.
     *  \param att the attributes to print.
     *  \return the output stream for chaining.
     *  \tparam AttrSeq A boost mpl list of Attributes.
     */
    template< class AttrSeq >
    inline ::logging::loggingReturnType &operator<<(loggingReturnType &out, const boost::shared_ptr<ComparableAttributeSet<AttrSeq> > &att) {
        att->print(out);
        return out;
    }

} /* namespace logging */
#endif /* _ComparableAttributeSet_ */
