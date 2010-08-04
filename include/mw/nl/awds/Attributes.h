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
#include "mw/attributes/Throughput.h"
#include "mw/attributes/PacketLoss.h"
#include "mw/attributes/filter/find.h"
#include "mw/attributes/filter/greater_than_or_equal_to.h"
#include "mw/attributes/filter/less_than_or_equal_to.h"
#include "mw/attributes/detail/AttributeSetProvider.h"
#include "mw/nl/awds/logging.h"
#include "mw/nl/awds/FlowId.h"

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

                    using namespace famouso::mw::attributes;
                    using famouso::mw::attributes::detail::SystemIDs;

                    /*!
                     * \brief Holds the IDs of the defined flow management %attributes
                     *  at one place.
                     *
                     *  \todo Change ids to be grater than SystemIDs::nonSystem when Bug #80 is closed.
                     */
                    struct FlowMgmtIDs {
                            enum {
                                action = 1, /**< The action attribute. */
                                flowId = action + 1, /**< The flow id attribute. */
                                subject = flowId + 1, /**< The subject attribute. */
                                prio = subject + 1
                            /**< The priority of the AWDS flow. */
                            };
                    };

                    /** \brief Ids used by the flow management action attribute.
                     */
                    struct FlowMgmtActionIDs {
                            enum {
                                reg = 1, /**< Register a new flow id */
                                free = 2, /**< Unregister a flow id. */
                                dead = 3, /**< Flow manager has killed a flow. */
                                use = 4, /**< Flow manager has registered a new flow. */
                                avail = 5, /**< Flow manager is available. */
                                max
                            /**< Only used by the attribute to determine the maximum size. */
                            };
                    };

                    /*!
                     * \brief Defines a configurable flow id attribute for
                     *  describing the requestet flow at AWDS.
                     *
                     * \tparam id Describes the initial value to be set
                     */
                    template< FlowId id >
                    class FlowMgmtID: public Attribute<FlowMgmtID<0> , tags::integral_const_tag, FlowId, id, filter::less_than_or_equal_to,
                                    FlowMgmtIDs::flowId, false> {
                        public:
                            typedef FlowMgmtID type;
                    };

                    /*!
                     * \brief Defines a configurable flow management action attribute for
                     *  describing the action to do at flow management at AWDS.
                     *
                     * \tparam id Describes the initial value to be set
                     */
                    template< uint8_t action >
                    class FlowMgmtAction: public Attribute<FlowMgmtAction<0> , tags::integral_const_tag, uint8_t, action,
                                    filter::less_than_or_equal_to, FlowMgmtIDs::action, false> {
                        public:
                            typedef FlowMgmtAction type;
                    };

                    /** \brief A attribute to hold a subject in an AttributeSet.
                     *
                     * \tparam snn The initial subject to set.
                     */
                    template< uint64_t snn >
                    class SubjectAttribute: public Attribute<SubjectAttribute<0> , tags::integral_const_tag, uint64_t, snn,
                                    filter::less_than_or_equal_to, FlowMgmtIDs::subject, false> {
                        public:
                            typedef SubjectAttribute type;

                            /** \brief Get the data as subject.
                             *
                             *  \return The subject.
                             */
                            Subject subject() {
                                return Subject(this->get());
                            }

                            /** \brief Sets the data to the given subject.
                             *
                             *  \param s The subject to set.
                             */
                            void subject(Subject s) {
                                this->set(s.value());
                            }
                    };

                    /*!
                     * \brief Defines a configurable flow management priority attribute for
                     *  describing the action to do at flow management at AWDS.
                     *
                     * \tparam prio Describes the initial value to be set
                     */
                    template< int8_t prio >
                    class Priority: public Attribute<Priority<0> , tags::integral_const_tag, int8_t, prio,
                                    filter::less_than_or_equal_to, FlowMgmtIDs::prio, false> {
                        public:
                            typedef Priority type;
                    };

                } // namespace detail

                using famouso::mw::attributes::detail::SetProvider;

                /** The Time-To-Live attribute. */
                typedef famouso::mw::attributes::TTL<0xFF> TTL;

                /** The %Latency attribute. */
                typedef famouso::mw::attributes::Latency<0xFFFFFFFF> Latency;

                /** The %Bandwidth attribute */
                typedef famouso::mw::attributes::Throughput<0xFFFFFFFF> Throughput;

                /** The Packet-Loss-Rate attribute. */
                typedef famouso::mw::attributes::PacketLoss<0xFFFF> PacketLoss;

                /** A AttributeSet of AWDS Attributes */
                typedef SetProvider<TTL, Latency, Throughput, PacketLoss>::attrSet AWDSAttributeSet;

                /** The flow id of the AWDS flow manager */
                typedef detail::FlowMgmtID<0xFFFFFFFF> FlowMgmtID;

                /** \brief Ids used by the flow management action attribute. */
                typedef detail::FlowMgmtActionIDs FlowMgmtActionIDs;

                /** The flow management action. */
                typedef detail::FlowMgmtAction<FlowMgmtActionIDs::max> FlowMgmtAction;

                /** The subject to register a flow for. */
                typedef detail::SubjectAttribute<0xFFFFFFFFFFFFFFFFull> SubjectAttribute;

                /** The AWDS Flow Priority. */
                typedef detail::Priority<0xFF> Priority;

                /** An attribute set used by the Famouso part of the flow management. */
                typedef SetProvider<FlowMgmtAction, SubjectAttribute, FlowMgmtID, Throughput, Priority>::attrSet FlowMgmtRequestAttributeSet;

                /** An attribute set used by the AWDS part of the flow mamagement. */
                typedef SetProvider<FlowMgmtAction, SubjectAttribute, FlowMgmtID>::attrSet FlowMgmtResponseAttributeSet;

#ifdef RANDOM_ATTRIBUTES

                /** \brief Create an attributes set of random attributes.
                 *  \return An attributes set.
                 */
                inline void _createAttrSet(uint8_t *data) {

                    switch (rand() % 11) {
                        case 0: {
                            new (data) SetProvider<TTL>::attrSet;
                            break;
                        }
                        case 1: {
                            new (data) SetProvider<Throughput>::attrSet;
                            break;
                        }
                        case 2: {
                            new (data) SetProvider<Latency>::attrSet;
                            break;
                        }
                        case 3: {
                            new (data) SetProvider<PacketLoss>::attrSet;
                            break;
                        }
                        case 4: {
                            new (data) SetProvider<TTL, PacketLoss>::attrSet;
                            break;
                        }
                        case 5: {
                            new (data) SetProvider<TTL, Throughput>::attrSet;
                            break;
                        }
                        case 6: {
                            new (data) SetProvider<TTL, Latency>::attrSet;
                            break;
                        }
                        case 7: {
                            new (data) SetProvider<Latency, PacketLoss>::attrSet;
                            break;
                        }
                        case 8: {
                            new (data) SetProvider<TTL, Throughput, PacketLoss>::attrSet;
                            break;
                        }
                        case 9: {
                            new (data) SetProvider<TTL, Latency, PacketLoss>::attrSet;
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
                inline AWDSAttributeSet::type createRandAttributes() {
                    AWDSAttributeSet::type res;
                    _createAttrSet(reinterpret_cast<uint8_t *> (&res));

                    TTL *ttl = res.find<TTL> ();
                    if (ttl)
                        ttl->set(rand() % 20 + 1);

                    Latency *l = res.find<Latency> ();
                    if (l)
                        l->set(rand() % 50 + 20);

                    Throughput *b = res.find<Throughput> ();
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
