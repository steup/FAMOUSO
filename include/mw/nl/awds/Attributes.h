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
#include "mw/attributes/Latency.h"
#include "mw/attributes/Bandwidth.h"
#include "mw/attributes/PacketLoss.h"
#include "mw/attributes/filter/find.h"
#include "mw/attributes/filter/greater_than_or_equal_to.h"
#include "mw/attributes/filter/less_than_or_equal_to.h"
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
                    /*!
                     * \brief Helper structs for printing the operator of a given
                     *  comparator.
                     */
                    template <typename Comparator>
                    struct op_printer {
                            static const char* op() {
                                return (" ? ");
                            }
                    };
                    template <>
                    struct op_printer<famouso::mw::attributes::filter::less_than_or_equal_to> {
                            static const char* op() {
                                return (" <= ");
                            }
                    };
                    template <>
                    struct op_printer<famouso::mw::attributes::filter::greater_than_or_equal_to> {
                            static const char* op() {
                                return (" >= ");
                            }
                    };

                } // namespace detail

                /** The Time-To-Live attribute. */
                typedef famouso::mw::attributes::TTL<0xFF> TTL;

                /** The %Latency attribute. */
                typedef famouso::mw::attributes::Latency<0xFFFFFFFF> Latency;

                /** The %Bandwidth attribute */
                typedef famouso::mw::attributes::Bandwidth<0xFFFFFFFF> Bandwidth;

                /** The Packet-Loss-Rate attribute. */
                typedef famouso::mw::attributes::PacketLoss<0xFFFF> PacketLoss;

                /** A list of AWDS Attributes */
                typedef boost::mpl::list<TTL, Latency, Bandwidth, PacketLoss>::type AWDSAttributesList;

                /** A AttributeSet of AWDS Attributes */
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
                inline void _createAttrSet(uint8_t *data) {
                    using boost::mpl::list;

                    switch (rand() % 11) {
                        case 0: {
                            new (data) _aset<list<TTL> >::t;
                            break;
                        }
                        case 1: {
                            new (data) _aset<list<Bandwidth> >::t;
                            break;
                        }
                        case 2: {
                            new (data) _aset<list<Latency> >::t;
                            break;
                        }
                        case 3: {
                            new (data) _aset<list<PacketLoss> >::t;
                            break;
                        }
                        case 4: {
                            new (data) _aset<list<TTL, PacketLoss> >::t;
                            break;
                        }
                        case 5: {
                            new (data) _aset<list<TTL, Bandwidth> >::t;
                            break;
                        }
                        case 6: {
                            new (data) _aset<list<TTL, Latency> >::t;
                            break;
                        }
                        case 7: {
                            new (data) _aset<list<Latency, PacketLoss> >::t;
                            break;
                        }
                        case 8: {
                            new (data) _aset<list<TTL, Bandwidth, PacketLoss> >::t;
                            break;
                        }
                        case 9: {
                            new (data) _aset<list<TTL, Latency, PacketLoss> >::t;
                            break;
                        }
                        default:
                            break;
                    }
                }

                /*! \brief Creates a random attributes instance.
                 *
                 *  \return An instance of attributes.
                 */
                inline AWDSAttributesSet::type createRandAttributes() {
                    AWDSAttributesSet::type res;
                    createAttrSet(reinterpret_cast<uint8_t *> (&res));

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
