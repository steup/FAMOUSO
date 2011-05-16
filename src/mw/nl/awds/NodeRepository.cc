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

#include "mw/nl/awds/logging.h"
#include "mw/nl/awds/NodeRepository.h"

#include <climits>

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {

                NodeRepository & NodeRepository::getInstance() {
                    static NodeRepository instance;
                    return instance;
                }

                NodeRepository::NodeRepository() {
                }

                Node::type NodeRepository::find(MAC &mac) {
                    //log::emit<AWDS>() << "Searching node ... ";

                    // look for a node with specified mac

                    for (NodeList::iterator it = _nodes.begin(); it != _nodes.end(); it++) {
                        if ((*it)->mac() == mac) {
                            //log::emit() << "found: " << *it << log::endl;
                            return *it;
                        }
                    }

                    // node not found, create a new and register
                    Node::type node = Node::create(mac);
                    //log::emit() << "not found: " << node << log::endl;
                    _nodes.push_back(node);
                    return node;
                }

                std::pair<NodeRepository::NodeIterator, NodeRepository::NodeIterator> NodeRepository::find(SNN subject) {
                    log::emit<AWDS>() << "Searching nodes for subject (" << subject << ") ... " << log::endl;

                    // list of subscribers which match all attributes
                    boost::shared_ptr<NodeList> result(new NodeList());

                    // look for registered subject
                    SubscriberMap::iterator sit = _snnmap.find(subject);

                    // subject not registered, return empty list
                    if (sit == _snnmap.end()) {
                        log::emit<AWDS>() << "no nodes found." << log::endl;
                        return boost::make_shared_container_range(result);
                    }

                    // list of all subscribers to the given subject
                    boost::shared_ptr<NodeSubscriberMap> cls = sit->second;

                    log::emit<AWDS>() << "found " << log::dec << cls->size() << " nodes." << log::endl;

                    log::emit<AWDS>() << "Checking nodes ... " << log::endl;

                    // counter for debugging, counts nodes which doesn't match attributes
                    int bad_subscribers = 0;

                    // attributes of publischer
                    Attributes::type pubAttr = _snnAttribs[subject];

                    // add clients to result list
                    for (NodeSubscriberMap::iterator it = cls->begin(); it != cls->end(); it++) {
                        Node::type node = it->first; // the actual node to check
                        Attributes::type subAttr = it->second->attribs, // The subscriber attributes
                                        nodeAttr = node->attr(); // The actual network attributes

                        //log::emit<AWDS>() << "Node: " << node << " " << nodeAttr << log::endl;

                        // check if node is to old or network attributes doesn't match publisher attributes
                        if (!Attributes::match(nodeAttr, pubAttr)) {
                            // ignore subscriber
                            bad_subscribers++;
                            continue; // we don't have to check subscriber attributes
                        }

                        // match network attributes to subscriber attributes
                        if (Attributes::match(nodeAttr, subAttr))
                            // add good node
                            result->push_back(node);
                        else
                            bad_subscribers++;
                    }

                    log::emit<AWDS>() << log::dec << "Found " << bad_subscribers << " bad subscribers." << log::endl;

                    return boost::make_shared_container_range(result);
                }

                FlowId NodeRepository::flowid(Node::type &node, SNN subject) {
                    // look for registered subject
                    Subscriber::type s = findSub(node, subject);

                    if (s)
                        return s->flowId;

                    log::emit<AWDS>() << "Could not get flowid." << log::endl;
                    return -1; // node not registered to subject, don't request a flow id
                }

                int NodeRepository::elapsed(Node::type &node, SNN subject) {
                    Subscriber::type s = findSub(node, subject);

                    if (s)
                        return s->elapsed();

                    log::emit<AWDS>() << "Could not get elapsed seconds." << log::endl;
                    return INT_MAX;
                }

                void NodeRepository::remove(Node::type &node) {
                    //log::emit<AWDS>() << "Remove node: " << node << log::endl;
                    // unregister node from all subjects
                    unreg(node);

                    // remove node from node list
                    // look for a node with specified mac
                    for (NodeList::iterator it = _nodes.begin(); it != _nodes.end(); it++) {
                        if (*it == node) {
                            _nodes.erase(it);
                            return;
                        }
                    }
                }

                void NodeRepository::remove(SNN &subject) {
                    //log::emit<AWDS>() << "Remove subject: " << subject << log::endl;

                    SubscriberMap::iterator it = _snnmap.find(subject);

                    // subject registered, delete it
                    if (it != _snnmap.end()) {
                        it->second->clear(); // TODO: Do we have to do this?
                        _snnmap.erase(it);
                    }
                }

                void NodeRepository::reg(Node::type &client, SNN &subject, Attributes::type &attribs) {
                    //log::emit<AWDS>() << "Register node to subject: " << node << " | " << subject << log::endl;
                    // look for clients registered to given subject
                    SubscriberMap::iterator it = _snnmap.find(subject);

                    // subject not registered
                    if (it == _snnmap.end()) {
                        return;
                    }

                    // add node to subject
                    NodeSubscriberMap &nsm = *_snnmap[subject];
                    nsm[client] = Subscriber::Create(attribs);
                }

                void NodeRepository::reg(SNN subject, Attributes::type &attribs) {
                    //log::emit<AWDS>() << "Register subject: " << subject << log::endl;
                    // look for registered subject
                    SubscriberMap::iterator it = _snnmap.find(subject);

                    // subject not registered, register it
                    if (it == _snnmap.end()) {
                        _snnmap[subject] = boost::shared_ptr<NodeSubscriberMap>(new NodeSubscriberMap());
                        _snnAttribs[subject] = attribs;
                    }
                }

                void NodeRepository::unreg(Node::type &node) {
                    //log::emit<AWDS>() << "Unregister node from subjects: " << node << log::endl;

                    // remove node from all subjects, loop over all subjects
                    for (SubscriberMap::iterator it = _snnmap.begin(); it != _snnmap.end(); it++) {
                        unreg(node, it->first);
                    }
                }

                void NodeRepository::unreg(Node::type &node, SNN subject) {
                    //log::emit<AWDS>() << "Unregister node from subject: " << node << log::endl;

                    // find the subject
                    SubscriberMap::iterator it = _snnmap.find(subject);
                    if (it == _snnmap.end()) {
                        log::emit<AWDS>() << "Subject not found! Could not unregister node." << log::endl;
                        return;
                    }

                    // loop over all clients registered to subject
                    NodeSubscriberMap::iterator it2 = it->second->find(node);

                    if (it2 != it->second->end()) // node is registered to subject, so unregister it
                        it->second->erase(it2);
                }

                void NodeRepository::update(Node::type &node, SNN subject, FlowId fId) {
                    // look for registered subject
                    Subscriber::type s = findSub(node, subject);

                    if (s)
                        s->flowId = fId;
                    else
                        log::emit<AWDS>() << "Udate flow id failed! (" << fId << ")" << log::endl;
                }

                NodeRepository::Subscriber::type NodeRepository::findSub(Node::type &node, SNN subject) {
                    Subscriber::type res;
                    // look for registered subject
                    SubscriberMap::iterator sit = _snnmap.find(subject);

                    // subject not registered, return empty list
                    if (sit == _snnmap.end()) {
                        log::emit<AWDS>() << "Subject not found! (" << subject << ")" << log::endl;
                        return res; // subject not registered
                    }

                    NodeSubscriberMap::iterator sl = sit->second->find(node);

                    if (sl != sit->second->end())
                        return sl->second;

                    log::emit<AWDS>() << "Node with subject not found! (" << node << " | " << subject << ")" << log::endl;
                    return res;
                }
            } /* awds */
        } /* nl */
    } /* mw */
} /* famouso */
