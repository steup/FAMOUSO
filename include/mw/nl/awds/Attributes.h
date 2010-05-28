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

#ifndef _Attributes_h_
#define _Attributes_h_

#include <cstring>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/mpl/list.hpp>
#include <boost/type_traits.hpp>
#include "mw/nl/awds/AWDS_Packet.h"
#include "mw/attributes/tags/IntegralConstTag.h"
#include "mw/attributes/Attribute.h"
#include "mw/attributes/AttributeSet.h"
#include "mw/attributes/TTL.h"
#include "mw/attributes/filter/find.h"
#include "logging.h"

#ifdef RANDOM_ATTRIBUTES
#include <cstdlib>
#endif

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {
                /** \brief This namepsace contains helper functions and classes for the attributes.
                 *
                 * \todo The Attributes should be replaced by typedefs or usings later. They should not be deleted,
                 *       because they are uses at AWDS Famouso module. Or youz have to change the AWDS module accordingly.
                 */
                namespace detail {
                    using famouso::mw::attributes::Attribute;
                    using famouso::mw::attributes::TTL;
                    using famouso::mw::attributes::tags::integral_const_tag;

                    /*!\brief   defines a configurable Latency attribute.
                     *
                     * \tparam  lat describes the initial value to be set
                     */
                    template< uint16_t lat >
                    class Latency: public Attribute<Latency<0>, integral_const_tag, uint16_t, lat, 2, true> {
                    };

                    /*!\brief defines a configurable Bandwith attribute.
                     *
                     * \tparam bw describes the initial value to be set
                     */
                    template< uint32_t bw >
                    class Bandwidth: public Attribute<Bandwidth<0>, integral_const_tag, uint32_t, bw, 3, true> {
                    };

                    /*!\brief defines a configurable Packet-Loss-Rate attribute.
                     *
                     * \tparam pl describes the initial value to be set
                     */
                    template< uint16_t pl >
                    class PacketLoss: public Attribute<PacketLoss<0>, integral_const_tag, uint16_t, pl, 4, true> {
                    };

                } // namespace detail

                typedef detail::TTL<0xFF> TTL;

                typedef detail::Latency<0xFFFF> Latency;

                typedef detail::Bandwidth<0xFFFFFFFF> Bandwidth;

                typedef detail::PacketLoss<0xFFFF> PacketLoss;

                /** A list of AWDS Attributes */
                typedef boost::mpl::list<TTL, Latency , Bandwidth , PacketLoss >::type
                AWDSAttributesList;

                /* \brief A class for holding attributes of publisher, subscriber or network.
                 *
                 * \tparam AttrSeq A list of attributes.
                 */
                template< class AttrSeq >
                class AttributesList: boost::noncopyable {
                    private:
                        /** A attributes sequence for finding attributes in some data. */
                        typedef typename famouso::mw::attributes::AttributeSequence<AttrSeq>::type attr_seq;

                        /**
                         * An iterator for printing and checking a list of attributes.
                         *
                         * \tparam Seq A list of attributes.
                         * \tparam Itr An iterator to Seq.
                         */
                        template< class Seq, class Itr = typename boost::mpl::begin<Seq>::type >
                        class AttributeIterator {

                                /** The actual attribute type */
                                typedef typename boost::mpl::deref<Itr>::type attrib;

                                /** The type of the last attribute in the list Seq. */
                                typedef typename boost::mpl::end<Seq>::type end;

                                /* \brief Checks weather the given attributes are matching.
                                 *
                                 * \param a The attribute wich has to be less or equal to the b.
                                 * \param b The attribute wich has to be more or equal to a or null.
                                 * \return Returns true if b is null or a is less or equal to b, otherwise false.
                                 */
                                static bool match(const attrib *a, const attrib *b) {
                                    // if attrib b is empty check is always true
                                    if (!b)
                                        return true;

                                    // if attrib b is not empty, attrib a has to be not empty too, so check if it matches
                                    if (a && a->get() <= b->get())
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
                                static bool test(attr_seq &a, attr_seq &b) {
                                    // find attributes
                                    attrib *l = a.template find<attrib> ();
                                    attrib *r = b.template find<attrib> ();

                                    log::emit<ATTR>() << "matching " << (int) attrib::id << ": " << (int) (l ? l->get() : -1) << " <= "
                                                    << (int) (r ? r->get() : -1) << log::endl;

                                    // match attribute
                                    if (!match(l, r)){
                                        log::emit<ATTR>() << "unsuccessfull" << log::endl;
                                        return false; // if attribs not match
                                    }

                                    // Check the next attribute in list
                                    return AttributeIterator<Seq, typename boost::mpl::next<Itr>::type>::test(a, b);
                                }

                                /**
                                 * Prints all attributes to the output stream.
                                 *
                                 * \param out The output stream.
                                 * \param a The attribute list.
                                 */
                                static void print(::logging::loggingReturnType &out, attr_seq &a) {
                                    // find the attribute
                                    attrib *attr = a.template find<attrib> ();
                                    if (attr) {
                                        // found, so get value
                                        typename attrib::value_type val = attr->get();
                                        // print name and value
                                        out << " [" << (int) attrib::id << "]={" << (int) val << "}";
                                    }

                                    // print next attribute in list
                                    AttributeIterator<Seq, typename boost::mpl::next<Itr>::type>::print(out, a);
                                }
                        };

                        /**
                         * Specialized template for end of recursion.
                         *
                         * \tparam Seq The attribute list.
                         */
                        template< class Seq >
                        class AttributeIterator<Seq, typename boost::mpl::end<Seq>::type> {
                            public:

                                static bool test(const attr_seq &a, const attr_seq &b) {
                                    log::emit<ATTR>() << "successfull" << log::endl;
                                    return true;
                                }

                                static void print(::logging::loggingReturnType &out, const attr_seq &a) {
                                }
                        };

                        /* \brief Copy the data and search for attributes
                         *
                         * \param d the data pointer
                         * \param s the size of the data
                         *
                         * \todo Copy only attributes, and not the whole data.
                         */
                        void set(uint8_t *d, uint16_t s) {

                            std::memcpy(data, d, s);

                        }

                    public:
                        /** \copydoc Attributes */
                        typedef boost::shared_ptr<AttributesList> type;

                        /**
                         * \brief Copy the attributes from the packet to this attribute instance.
                         *
                         * \param p The packet to copy from.
                         */
                        void set(awds::AWDS_Packet &p) {
                            // if this is not an attributes packet
                            if (p.header.type != AWDS_Packet::constants::packet_type::attributes)
                                return;

                            set(p.data, ntohs(p.header.size));
                        }

                        /*! \brief Checks weather all attributes are matching
                         *
                         *  If an attribute in o is null, the check for this attribute will always match.
                         *  The left hand parameter should be the actual network attributes, the right hand parameter
                         *  should be the publisher or subscriber defined attributes.
                         *
                         *  \param o The other attributes.
                         *  \return true if all attributes of this are less than or equal to o, otherwise false.
                         */
                        bool operator<=(const AttributesList & o) const {
                            attr_seq * attribs = reinterpret_cast<attr_seq*> (const_cast<uint8_t *> (data));
                            attr_seq * attribs_b = reinterpret_cast<attr_seq*> (const_cast<uint8_t *> (o.data));
                            return AttributeIterator<AttrSeq>::test(*attribs, *attribs_b);
                        }

                        /*! \brief print the attributes to the given stream.
                         *
                         *  \param out the output stream to print to.
                         */
                        void print(::logging::loggingReturnType &out) const {
                            attr_seq * attribs = reinterpret_cast<attr_seq*> (const_cast<uint8_t *> (data));
                            out << ::logging::log::dec;
                            AttributeIterator<AttrSeq>::print(out, *attribs);
                        }

                        /*! \brief Creates an empty attributes instance.
                         *
                         *  \return An instance of attributes.
                         */
                        static type create() {
                            type res = type(new AttributesList());
                            return res;
                        }

                        /*! \brief Creates an empty attributes instance.
                         *
                         *  \param p An AWDS_Packet to load attributes from.
                         *  \return An instance of attributes.
                         */
                        static type create(AWDS_Packet &p) {
                            type res = type(new AttributesList());
                            res->set(p);
                            return res;
                        }

#ifdef RANDOM_ATTRIBUTES
                        /*! \brief Creates a random attributes instance.
                         *
                         *  \return An instance of attributes.
                         */
                        static type createRand() {
                            type res = type(new AttributesList());

                            attr_seq *as = new (&res->data[0]) attr_seq();

                            if (rand() % 2 == 0) {
                                TTL *ttl = as->template find<TTL> ();
                                if (ttl)
                                    ttl->set(rand() % 20 + 1);
                            }

                            if (rand() % 2 == 0) {
                                Latency *l = as->template find<Latency> ();
                                if (l)
                                    l->set(rand() % 50 + 20);
                            }

                            /*if (rand() % 2 == 0) {
                             Bandwidth *b = as->template find<Bandwidth>();
                             if (b)
                             b->set((rand() % 1000 + 1000));
                             }*/

                            if (rand() % 2 == 0) {
                                PacketLoss *p = as->template find<PacketLoss> ();
                                if (p)
                                    p->set(rand() % 20 + 1);
                            }

                            return res;
                        }
#endif

                    private:
                        uint8_t data[AWDS_Packet::constants::packet_size::payload]; /**< The data where the attributes are stored. */
                };

                typedef AttributesList<AWDSAttributesList::type> Attributes;

                /*! \brief Checks weather attributes are matching
                 *
                 *  The left hand parameter should be the actual network attributes, the right hand parameter
                 *  should be the publisher or subscriber defined attributes.
                 *
                 *  Attributes::type is a typedef of boost::shared_ptr<Attributes>.
                 *  \param a A shared pointer to the first attributes.
                 *  \param b A shared pointer to the second attributes.
                 *  \return true if all attributes of a are less than or equal to b, otherwise false.
                 */
                inline bool operator<=(const Attributes::type &a, const Attributes::type &b) {
                    // so we have to dereference and compare
                    return *a <= *b;
                }

            } /* awds */
        } /* nl */
    } /* mw */
} /* famouso */

namespace logging {

    /*! \brief Print the attributes to the given stream.
     *
     *  Attributes::type is a typedef of boost::shared_ptr<Attributes>.
     *
     *  \param out the output stream to print to.
     *  \param att the attributes to print.
     *  \return the output stream for chaining.
     */
    inline ::logging::loggingReturnType &operator<<(loggingReturnType &out, const famouso::mw::nl::awds::Attributes::type &att) {
        att->print(out);
        return out;
    }

} // namespace logging
#endif /* _Attributes_h_ */
