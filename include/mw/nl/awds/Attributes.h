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

#include <boost/mpl/list.hpp>
#include "mw/attributes/tags/IntegralConstTag.h"
#include "mw/attributes/Attribute.h"
#include "mw/attributes/AttributeSet.h"
#include "mw/attributes/TTL.h"
#include "mw/nl/awds/logging.h"

#ifdef RANDOM_ATTRIBUTES
#include <cstdlib>
#endif

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {
                /** \brief This namepsace contains helper functions and classes for the attributes.
                 *
                 * \todo The elements at this namespace should moved to the right places later.
                 */
                namespace detail {
                    using famouso::mw::attributes::Attribute;
                    using famouso::mw::attributes::tags::integral_const_tag;

                    /** \brief Tag to compare Attributes.
                     *  The first attribute has to be less or equal to the second attribute.
                     *  \tparam Attrib The attribute type.
                     */
                    template< class Attrib >
                    struct LessThanTag {

                            /* \brief Checks wether the given attributes are matching.
                             *
                             * \param a The attribute wich has to be less or equal to the b.
                             * \param b The attribute wich has to be more or equal to a or null.
                             * \return Returns true if b is null or a is less or equal to b, otherwise false.
                             */
                            static bool match(const Attrib *a, const Attrib *b) {
                                // if attrib b is empty check is always true
                                if (!b)
                                    return true;

                                // if attrib b is not empty, attrib a has to be not empty too, so check if it matches
                                if (a && a->get() <= b->get())
                                    return true;

                                // attributes doesn't match
                                return false;
                            }

                            /** \brief The operator as string for debugging purposes.
                             *  \return A string version of the operator <=.
                             */
                            static const char *op() {
                                return " <= ";
                            }
                    };

                    /** \brief Tag to compare Attributes.
                     *  The first attribute has to be greater or equal to the second attribute.
                     *  \tparam Attrib The attribute type.
                     */
                    template< class Attrib >
                    struct GreaterThanTag {

                            /* \brief Checks wether the given attributes are matching.
                             *
                             * \param a The attribute wich has to be greater or equal to the b.
                             * \param b The attribute wich has to be more or equal to a or null.
                             * \return Returns true if b is null or a is greater or equal to b, otherwise false.
                             */
                            static bool match(const Attrib *a, const Attrib *b) {
                                // if attrib b is empty check is always true
                                if (!b)
                                    return true;

                                // if attrib b is not empty, attrib a has to be not empty too, so check if it matches
                                if (a && a->get() >= b->get())
                                    return true;

                                // attributes doesn't match
                                return false;
                            }
                            /** \brief The operator as string for debugging purposes.
                             *  \return A string version of the operator >=.
                             */
                            static const char *op() {
                                return " >= ";
                            }
                    };

                    /*!\brief   defines a configurable Time-To-Live attribute.
                     *
                     * \tparam  ttl describes the initial value to be set
                     */
                    template< uint8_t ttl >
                    class TTL: public famouso::mw::attributes::TTL<ttl>, public LessThanTag<TTL<ttl> > {
                    };

                    /*!\brief   defines a configurable Latency attribute.
                     *
                     * \tparam  lat describes the initial value to be set
                     */
                    template< uint16_t lat >
                    class Latency: public Attribute<Latency<0> , integral_const_tag, uint16_t, lat, 2, true> , public LessThanTag<Latency<
                                    lat> > {
                    };

                    /*!\brief defines a configurable Bandwith attribute.
                     *
                     * \tparam bw describes the initial value to be set
                     */
                    template< uint32_t bw >
                    class Bandwidth: public Attribute<Bandwidth<0> , integral_const_tag, uint32_t, bw, 3, true> , public GreaterThanTag<
                                    Bandwidth<bw> > {
                    };

                    /*!\brief defines a configurable Packet-Loss-Rate attribute.
                     *
                     * \tparam pl describes the initial value to be set
                     */
                    template< uint8_t pl >
                    class PacketLoss: public Attribute<PacketLoss<0> , integral_const_tag, uint8_t, pl, 4, true> , public LessThanTag<
                                    PacketLoss<pl> > {
                    };

                } // namespace detail

                /** The Time-To-Live attribute. */
                typedef detail::TTL<0xFF> TTL;

                /** The %Latency attribute. */
                typedef detail::Latency<0xFFFF> Latency;

                /** The %Bandwidth attribute */
                typedef detail::Bandwidth<0xFFFFFFFF> Bandwidth;

                /** The Packet-Loss-Rate attribute. */
                typedef detail::PacketLoss<0xFF> PacketLoss;

                /** A list of AWDS Attributes */
                typedef boost::mpl::list<TTL, Latency, Bandwidth, PacketLoss>::type AWDSAttributesList;

                /** A AttributeSequence of AWDS Attributes */
                typedef famouso::mw::attributes::AttributeSet<AWDSAttributesList::type>::type AWDSAttributesSet;

#ifdef RANDOM_ATTRIBUTES

                /** \brief Only used internally for simplify writing.
                 */
                template< typename AttrSet >
                struct _aset {
                        typedef famouso::mw::attributes::AttributeSet<AttrSet> t;
                };

                /** \brief Create an attributes set of random attributes.
                 *  \return An attributes set.
                 */
                inline AWDSAttributesSet::type _createAttrSet() {
                    using boost::mpl::list;
                    typedef AWDSAttributesSet::type rt;
                    rt res;

                    switch (rand() % 11) {
                        case 0: {
                            _aset<list<TTL> >::t attr;
                            res = *reinterpret_cast<rt *> (&attr);
                            break;
                        }
                        case 1: {
                            _aset<list<Bandwidth> >::t attr;
                            res = *reinterpret_cast<rt *> (&attr);
                            break;
                        }
                        case 2: {
                            _aset<list<Latency> >::t attr;
                            res = *reinterpret_cast<rt *> (&attr);
                            break;
                        }
                        case 3: {
                            _aset<list<PacketLoss> >::t attr;
                            res = *reinterpret_cast<rt *> (&attr);
                            break;
                        }
                        case 4: {
                            _aset<list<TTL, PacketLoss> >::t attr;
                            res = *reinterpret_cast<rt *> (&attr);
                            break;
                        }
                        case 5: {
                            _aset<list<TTL, Bandwidth> >::t attr;
                            res = *reinterpret_cast<rt *> (&attr);
                            break;
                        }
                        case 6: {
                            _aset<list<TTL, Latency> >::t attr;
                            res = *reinterpret_cast<rt *> (&attr);
                            break;
                        }
                        case 7: {
                            _aset<list<Latency, PacketLoss> >::t attr;
                            res = *reinterpret_cast<rt *> (&attr);
                            break;
                        }
                        case 8: {
                            _aset<list<TTL, Bandwidth, PacketLoss> >::t attr;
                            res = *reinterpret_cast<rt *> (&attr);
                            break;
                        }
                        case 9: {
                            _aset<list<TTL, Latency, PacketLoss> >::t attr;
                            res = *reinterpret_cast<rt *> (&attr);
                            break;
                        }
                        default:
                            break;
                    }
                    return res;
                }

                /*! \brief Creates a random attributes instance.
                 *
                 *  \return An instance of attributes.
                 */
                inline AWDSAttributesSet::type createRandAttributes() {
                    AWDSAttributesSet::type res = _createAttrSet();

                    TTL *ttl = res.find<TTL> ();
                    if (ttl)
                        ttl->set(rand() % 20 + 1);

                    Latency *l = res.find<Latency> ();
                    if (l)
                        l->set(rand() % 50 + 20);

                    Bandwidth *b = res.find<Bandwidth> ();
                    if (b)
                        b->set((rand() % 1000 + 1000));

                    PacketLoss *p = res.find<PacketLoss> ();
                    if (p)
                        p->set(rand() % 20 + 1);

                    return res;
                }
#endif

            } /* awds */
        } /* nl */
    } /* mw */
} /* famouso */

#endif /* _Attributes_h_ */
