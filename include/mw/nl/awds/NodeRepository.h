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
#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"
#include "mw/common/Subject.h"
#include "mw/nl/awds/MAC.h"
#include "mw/nl/awds/Node.h"
#include "mw/nl/awds/Attributes.h"
#include "mw/nl/awds/ComparableAttributesSet.h"
#include "mw/nl/awds/lists.h"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {

                typedef ComparableAttributesSet<AWDSAttributesList::type> Attributes;

                /*! \brief A node repository for holding AWDS nodes and register nodes to subjects.
                 *
                 * \todo We can't handle multiple publisher with same subject and different attributes.
                 *       We use the attributes of latest announce for now.
                 */
                class NodeRepository: boost::noncopyable {

                    public:

                        /** \copydoc NodeRepository */
                        typedef NodeRepository type;

                        /** \brief A List for holding Nodes */
                        typedef Container<Node::type> NodeList;

                        /*! \brief A famouso Subject.
                         */
                        typedef famouso::mw::Subject SNN;

                    private:

                        /*! \brief A class for mapping attributes to a node.
                         */
                        class Subscriber {
                            public:
                                /** \brief Creates a new Subcriber with the given parameters.
                                 *
                                 * \param c The node on which the subscriber resides.
                                 * \param a Attributes, which can be from network or subscriber.
                                 * \return A new Subscriber instance.
                                 */
                                static Subscriber Create(Node::type c, Attributes::type a) {
                                    Subscriber res;
                                    res.node = c;
                                    res.attribs = a;
                                    return res;
                                }

                                Node::type node; /**<  The node on which the subscriber resides. */
                                Attributes::type attribs; /**< The attributes from network or subscriber */

                        };

                        /*! \brief A list to hold subscribers with their attributes.
                         */
                        typedef Container<Subscriber> SubscriberList;

                        /*! \brief A map to assign clients to subjects.
                         */
                        typedef std::map<SNN, SubscriberList::type> SubscriberMap;

                        /*! \brief A map to assign attributes to subjects.
                         */
                        typedef std::map<SNN, Attributes::type> PublisherMap;

                        /** \brief Constructor to init lists. */
                        NodeRepository();

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
                        Node::type find(MAC mac);

                        /*! \brief Find all nodes registered to the given Subject wich match the required attributes.
                         *         The attributes can be defined by publisher or subscriber.
                         *
                         *  \param subject The subject to find nodes for.
                         *  \return A list of nodes registered for the given subject.
                         *          The list can be empty, if no node is registered or no node match the required
                         *          attributes.
                         */
                        NodeList::type find(SNN subject);

                        /*! \brief Remove a node from the repository.
                         *         It will be removed from all subjects too.
                         *
                         *  \param node The node to remove.
                         */
                        void remove(Node::type node);

                        /*! \brief Remove a subject from the repository.
                         *         Clients registered to the given subject will
                         *         be unregistered from it.
                         *
                         *  \param subject The subject to remove.
                         */
                        void remove(SNN subject);

                        /*! \brief Register a node to a subject.
                         *
                         *  \param node The node to register.
                         *  \param subject The subject to register.
                         *  \param attribs The attributes which the subscriber requires.
                         *
                         *  \todo If the subject is not registered, this function should do nothing.
                         */
                        void reg(Node::type node, SNN subject, Attributes::type attribs = Attributes::create());

                        /*! \brief Register a subject to the repository.
                         *
                         *  \param subject The subject to register.
                         *  \param attribs The attributes which the publisher requires.
                         */
                        void reg(SNN subject, Attributes::type attribs = Attributes::create());

                        /*! \brief Unregister the given node from all known subjects.
                         *
                         *  \param node The node to unregister.
                         */
                        void unreg(Node::type node);

                        /*! \brief The maximum time when a node has to be resubscribe.
                         *
                         *         The default value is 70 seconds.
                         *
                         *  \param age The time in seconds.
                         */
                        void maxAge(int age);

                        /*! \brief Update attribute of given node.
                         *
                         *  This function saves the given network attributes to the given node.
                         *  \param node The node to update.
                         *  \param attribs The new attributes to set.
                         */
                        void update(Node::type node, Attributes::type attribs = Attributes::create());

                    private:
                        SubscriberMap _snnmap; /**< A map to assign nodes with attributes to subjects. */
                        SubscriberList::type _nodes; /**< A list wich hold all known nodes at the AWDS network. */
                        PublisherMap _snnAttribs; /**< A map to assign attributes to a subject. */
                        int _maxAge /**< The timespan in seconds when a node is marked as offline. */;
                };

            } /* namespace awds */
        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */
#endif /* __ClientRepository_hpp__ */
