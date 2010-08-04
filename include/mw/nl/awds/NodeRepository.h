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
#ifndef __NodeRepository_hpp__
#define __NodeRepository_hpp__

#include <map>
#include <list>
#include <ctime>
#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/shared_container_iterator.hpp"
#include <boost/mpl/for_each.hpp>
#include "mw/common/Subject.h"
#include "mw/nl/awds/MAC.h"
#include "mw/nl/awds/Node.h"
#include "mw/nl/awds/Attributes.h"
#include "mw/nl/awds/ComparableAttributeSet.h"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {

                /** A list of attributes to check before publishing. */
                typedef ComparableAttributeSet<AWDSAttributeSet::type> Attributes;

                /** A node with Attributes. Here we use AWDS attributes. */
                typedef detail::Node<AWDSAttributeSet::type> Node;

                /*! \brief A node repository for holding AWDS nodes and register nodes to subjects.
                 *
                 * \todo We can't handle multiple publisher with same subject and different attributes.
                 *       We use the attributes of latest announce for now.
                 */
                class NodeRepository: boost::noncopyable {

                    public:

                        /** \copydoc NodeRepository */
                        typedef NodeRepository type;

                        typedef boost::shared_container_iterator<std::list<Node::type> > NodeIterator;

                        /*! \brief A famouso Subject.
                         */
                        typedef famouso::mw::Subject SNN;

                    private:

                        /*! \brief A class for mapping attributes to a node.
                         */
                        class Subscriber: boost::noncopyable {

                                clock_t _time; /**< The timstamp of last subscription from the node. */

                                Subscriber() :
                                    _time(std::clock()) {
                                }

                            public:

                                /** \brief This type secured by a shared pointer.
                                 */
                                typedef boost::shared_ptr<Subscriber> type;

                                /** \brief Creates a new Subcriber with the given parameters.
                                 *
                                 * \param c The node on which the subscriber resides.
                                 * \param a Attributes, which are from a subscriber.
                                 * \param fId The flow id of the subscriber.
                                 * \return A new Subscriber instance.
                                 */
                                static type Create(Attributes::type a, FlowId fId = 0) {
                                    type res = type(new Subscriber());
                                    res->attribs = a;
                                    res->flowId = fId;
                                    return res;
                                }

                                /*! \brief The time in seconds when the node was last seen.
                                 *
                                 * \return The elapsed time in seconds.
                                 */
                                int elapsed() const {
                                    return static_cast<int> (std::clock() - _time);
                                }

                                Attributes::type attribs; /**< The attributes from a subscriber. */
                                FlowId flowId; /**< The AWDS flow id. */
                        };

                        /** \brief A function object for finding attributes and set the strictest to the result.
                         *
                         * The template parameter defines the attribute to search for.
                         *
                         * \tparam AttrSet The attributes set of the result type.
                         */
                        template< class AttrSet >
                        class attr_finder {
                                /** The boost sequence of the result set. */
                                typedef typename AttrSet::sequence Seq;

                                AttrSet &_res; /**< The result attributes set. */
                                Attributes::type &_pub, /**< The publisher attributes set. */
                                &_sub; /**< The subscriber attributes set. */

                                /** \brief Creates a new function object for use with boost::mpl::for_each.
                                 *
                                 *  \param res The resulting attribute set.
                                 *  \param pub The pubischer attribute set.
                                 *  \param sub The subscriber attribute set.
                                 */
                                attr_finder(AttrSet &res, Attributes::type &pub, Attributes::type &sub) :
                                    _res(res), _pub(pub), _sub(sub) {
                                }

                            public:

                                /** \brief Apply's the function object with the given parameters.
                                 *
                                 *  \param res The resulting attribute set.
                                 *  \param pub The pubischer attribute set.
                                 *  \param sub The subscriber attribute set.
                                 */
                                static void apply(AttrSet &res, Attributes::type &pub, Attributes::type &sub) {
                                    boost::mpl::for_each<Seq>(attr_finder(res, pub, sub));
                                }

                                /** \brief Finds the attributes and sets the strictest value to the result attribute.
                                 *
                                 *  \param a Dummy, only the type is needed.
                                 */
                                template< class Attrib >
                                void operator()(Attrib a) {
                                    /** The actual attribute comparator. */
                                    typedef typename Attrib::comparator cmp;

                                    if (!Attrib::isSystem)
                                        return; // Only system attributes are searched.

                                    // The resulting attribute
                                    Attrib *r = _res.template find<Attrib> ();
                                    // the publisher attribute
                                    Attrib *p = _pub->template find<Attrib> ();
                                    // the subscriber attribute
                                    Attrib *s = _sub->template find<Attrib> ();

                                    if (p) {
                                        // publisher attibute is defined
                                        if (s && cmp::apply_runtime(p->get(), s->get()))
                                            // subscriber attribute is also defined and stricter, so use this
                                            r->set(s->get());
                                        else
                                            // subscriber attribute not defined or not stricter, so use publisher attribute
                                            r->set(p->get());
                                    } else if (s) {
                                        // only subscriber attibute is defined
                                        r->set(s->get());
                                    } else
                                        log::emit<AWDS>() << "Missing attribute with id " << (int) Attrib::id << log::endl;
                                }
                        };

                        /** \brief A List for holding Nodes */
                        typedef std::list<Node::type> NodeList;

                        /*! \brief A list to hold subscribers with their attributes.
                         */
                        typedef std::map<Node::type, Subscriber::type, Node::comp> NodeSubscriberMap;

                        /*! \brief A map to assign clients to subjects.
                         */
                        typedef std::map<SNN, boost::shared_ptr<NodeSubscriberMap> > SubscriberMap;

                        /*! \brief A map to assign attributes to subjects.
                         */
                        typedef std::map<SNN, Attributes::type> PublisherMap;

                        /** \brief A map to assign network attributes to nodes.
                         */
                        //typedef std::list<Node::type, Node::comp> NodeList;

                        /** \brief Constructor to init lists. */
                        NodeRepository();

                        Subscriber::type findSub(Node::type &node, SNN subject);

                    public:

                        /*! \brief Get the only instance of the node repository.
                         *
                         *  \return A reference to the only instance of the repository.
                         */
                        static type& getInstance();

                        /*! \brief Find a node by a given MAC address.
                         *         If the node doesn't exists, it will be created.
                         *
                         *  \param mac The MAC address to find the node for.
                         *  \return The node corresponding to the MAC address.
                         */
                        Node::type find(MAC &mac);

                        /*! \brief Find all nodes registered to the given Subject wich match the required attributes.
                         *         The attributes can be defined by publisher or subscriber.
                         *
                         *  \param subject The subject to find nodes for.
                         *  \return A list of nodes registered for the given subject.
                         *          The list can be empty, if no node is registered or no node match the required
                         *          attributes.
                         */
                        std::pair<NodeIterator, NodeIterator> find(SNN subject);

                        /** \brief Find the flow id for a node subscribed to a subject.
                         *
                         *  \param node The node to find the flow id of.
                         *  \param subject The subject to find the flow id of.
                         *  \return The found flow id or -1.
                         */
                        FlowId flowid(Node::type &node, SNN subject);

                        /** \brief Find the elapsed seconds since a node subscribed to a subject.
                         *
                         *  \param node The node to find the flow id of.
                         *  \param subject The subject to find the flow id of.
                         *  \return The found seconds or INT_MAX if not found.
                         */
                        int elapsed(Node::type &node, SNN subject);

                        /** \brief Search the subscriber and publisher attributes defined by the given attribute set and set
                         *         the strictest to the given set.
                         *
                         *  \param node The node to find the attributes of.
                         *  \param subject The subject to find the attributes of.
                         *  \param cas The attributes set to save the found attributes.
                         *  \tparam AttrSet The atttributes set type.
                         */
                        template< class AttrSet >
                        void find(Node::type &node, SNN subject, AttrSet &cas) {
                            Attributes::type subAttr, pubAttr = _snnAttribs[subject];

                            // find the subsciber
                            Subscriber::type s = findSub(node, subject);
                            if (s) // we found the subscriber
                                subAttr = s->attribs; // get the subscriber attributes

                            if (pubAttr && subAttr)
                                attr_finder<AttrSet>::apply(cas, pubAttr, subAttr);
                            else
                                log::emit<AWDS>() << "Missing attributes of subscriber or publisher." << log::endl;
                        }

                        /*! \brief Remove a node from the repository.
                         *         It will be removed from all subjects too.
                         *
                         *  \param node The node to remove.
                         */
                        void remove(Node::type &node);

                        /*! \brief Remove a subject from the repository.
                         *         Clients registered to the given subject will
                         *         be unregistered from it.
                         *
                         *  \param subject The subject to remove.
                         */
                        void remove(SNN &subject);

                        /*! \brief Register a node to a subject.
                         *
                         *  \param node The node to register.
                         *  \param subject The subject to register.
                         *  \param attribs The attributes which the subscriber requires.
                         *
                         *  \todo If the subject is not registered, this function should do nothing.
                         */
                        void reg(Node::type &node, SNN &subject, Attributes::type &attribs);

                        /*! \brief Register a subject to the repository.
                         *
                         *  \param subject The subject to register.
                         *  \param attribs The attributes which the publisher requires.
                         */
                        void reg(SNN subject, Attributes::type &attribs);

                        /*! \brief Unregister the given node from all known subjects.
                         *
                         *  \param node The node to unregister.
                         */
                        void unreg(Node::type &node);

                        /*! \brief Unregister the given node from a subject.
                         *
                         *  \param node The node to unregister.
                         *  \param subject The subject to unregister from.
                         */
                        void unreg(Node::type &node, SNN subject);

                        /** \brief Updates the flow id of a node registered to a subject.
                         *
                         *  \param node The node to update.
                         *  \param subject The subject to update.
                         *  \param fId The new flow id.
                         */
                        void update(Node::type &node, SNN subject, FlowId fId);

                    private:
                        SubscriberMap _snnmap; /**< A map to assign nodes with attributes to subjects. */
                        NodeList _nodes; /**< A list wich hold all known nodes and their network attributes at the AWDS network. */
                        PublisherMap _snnAttribs; /**< A map to assign attributes to a subject. */
                };

            } /* namespace awds */
        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */
#endif /* __ClientRepository_hpp__ */
