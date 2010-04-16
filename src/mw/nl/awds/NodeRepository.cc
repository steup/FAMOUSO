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

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {

                NodeRepository & NodeRepository::getInstance() {
                    static NodeRepository instance;
                    return instance;
                }

                NodeRepository::NodeRepository() :
                    _nodes(SubscriberList::Create()), _maxAge(70) {
                }

                Node::type NodeRepository::find(MAC mac) {
                    log::emit<AWDS>() << "Searching node ... ";
                    // look for a node with specified mac
                    for (SubscriberList::iterator it = _nodes->begin(); it != _nodes->end(); it++) {
                        if (mac == it->node->mac()) {
                            log::emit() << "found: " << it->node << log::endl;
                            return it->node;
                        }
                    }

                    // node not found, create a new and register
                    Node::type result = Node::create(mac);
                    log::emit() << "not found: " << result << log::endl;
                    _nodes->add(Subscriber::Create(result, Attributes::create()));
                    return result;
                }

                NodeRepository::NodeList::type NodeRepository::find(SNN subject) {
                    log::emit<AWDS>() << "Searching nodes for subject (" << subject << ") ... " << log::endl;

                    // list of subscribers which match all attributes
                    NodeList::type result = NodeList::Create();

                    // look for registered subject
                    SubscriberMap::iterator sit = _snnmap.find(subject);

                    // subject not registered, return empty list
                    if (sit == _snnmap.end()) {
                        log::emit<AWDS>() << "no nodes found." << log::endl;
                        return result;
                    }

                    // list of all subscribers to the given subject
                    SubscriberList::type cls = sit->second;

                    log::emit<AWDS>() << "found " << log::dec << cls->size() << " nodes." << log::endl;

                    log::emit<AWDS>() << "Checking nodes ... " << log::endl;

                    // counter for debugging, counts nodes which doesn't match attributes
                    int bad_subscribers = 0;

                    // attributes of publischer
                    Attributes::type pubAttr = _snnAttribs[subject];

                    // add clients to result list
                    for (SubscriberList::iterator it = cls->begin(); it != cls->end(); it++) {
                        // nodeA are the actual network attributes, subAttr are the subscriber attributes
                        Attributes::type subAttr, nodeAttr = it->attribs;

                        // check if node is to old or network attributes doesn't match publisher attributes
                        if ((it->node->elapsed() > _maxAge) || !(nodeAttr <= pubAttr)) {
                            // ignore subscriber
                            bad_subscribers++;
                            continue; // we don't have to check subscriber attributes
                        }

                        // find the attributes of subscriber
                        for (SubscriberList::iterator it2 = _nodes->begin(); it2 != _nodes->end(); it2++) {
                            if (it2->node == it->node) {
                                // attributes of subscriber found
                                subAttr = it2->attribs;
                                break; // we dont need to loop the rest
                            }
                        }

                        // match network attributes to subscriber attributes
                        if (nodeAttr <= subAttr)
                            // add good node
                            result->add(it->node);
                        else
                            bad_subscribers++;
                    }

                    log::emit<AWDS>() << log::dec << "Found " << bad_subscribers << " bad subscribers." << log::endl;

                    return result;
                }

                void NodeRepository::remove(Node::type node) {
                    log::emit<AWDS>() << "Remove node: " << node << log::endl;
                    // unregister node from all subjects
                    unreg(node);

                    // remove node from node list
                    for (SubscriberList::iterator it = _nodes->begin(); it != _nodes->end(); it++)
                        if (it->node == node) {
                            _nodes->erase(it);
                            break; // we don't nee to loop the rest
                        }
                }

                void NodeRepository::remove(SNN subject) {
                    log::emit<AWDS>() << "Remove subject: " << subject << log::endl;

                    SubscriberMap::iterator it = _snnmap.find(subject);

                    // subject registered, delete it
                    if (it != _snnmap.end()) {
                        it->second->clear(); // TODO: Do we have to do this?
                        _snnmap.erase(it);
                    }
                }

                void NodeRepository::reg(Node::type client, SNN subject, Attributes::type attribs) {
                    //log::emit<AWDS>() << "Register node to subject: " << node << " | " << subject << log::endl;
                    // look for clients registered to given subject
                    SubscriberMap::iterator it = _snnmap.find(subject);

                    // subject not registered
                    if (it == _snnmap.end()) {
                        // TODO: Temporary workaround until publisher announcing subjects
#ifdef RANDOM_ATTRIBUTES
                        reg(subject, Attributes::createRand());
#else
                        reg(subject, Attributes::create());
#endif
                    }

                    // add node to subject
                    _snnmap[subject]->add(Subscriber::Create(client, attribs));
                }

                void NodeRepository::reg(SNN subject, Attributes::type attribs) {
                    log::emit<AWDS>() << "Register subject: " << subject << log::endl;
                    // look for registered subject
                    SubscriberMap::iterator it = _snnmap.find(subject);

                    // subject not registered, register it
                    if (it == _snnmap.end()) {
                        _snnmap[subject] = SubscriberList::Create();
                        _snnAttribs[subject] = attribs;
                    }
                }

                void NodeRepository::unreg(Node::type node) {
                    log::emit<AWDS>() << "Unregister node from subjects: " << node << log::endl;

                    // remove node from all subjects, loop over all subjects
                    for (SubscriberMap::iterator it = _snnmap.begin(); it != _snnmap.end(); it++) {

                        // loop over all clients registered to subject
                        for (SubscriberList::iterator it2 = it->second->begin(); it2 != it->second->end(); it2++) {

                            // node is registered to subject, so unregister it
                            if (it2->node == node) {
                                it->second->erase(it2);
                                // node could not be registered more than on time
                                break;
                            }
                        }
                    }
                }

                void NodeRepository::maxAge(int age) {
                    _maxAge = age;
                }

                void NodeRepository::update(Node::type node, Attributes::type attribs) {
                    // look for a node with specified mac
                    for (SubscriberList::iterator it = _nodes->begin(); it != _nodes->end(); it++) {
                        if (node == it->node) {
                            it->attribs = attribs;
                            break;
                        }
                    }
                }
            } /* awds */
        } /* nl */
    } /* mw */
} /* famouso */
