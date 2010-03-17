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
#include "mw/nl/awds/ClientRepository.h"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {

                ClientRepository & ClientRepository::getInstance() {
                    static ClientRepository instance;
                    return instance;
                }

                ClientRepository::ClientRepository() :
                    _clients(SubscriberList::Create()), _maxAge(70) {
                }

                AWDSClient::type ClientRepository::find(MAC mac) {
                    log::emit<AWDS>() << "Searching client ... ";
                    // look for a client with specified mac
                    for (SubscriberList::iterator it = _clients->begin(); it != _clients->end(); it++) {
                        if (mac == it->client->mac()) {
                            log::emit() << "found: " << it->client << log::endl;
                            return it->client;
                        }
                    }

                    // client not found, create a new and register
                    AWDSClient::type result = AWDSClient::create(mac);
                    log::emit() << "not found: " << result << log::endl;
                    _clients->add(Subscriber::Create(result, Attributes::create()));
                    return result;
                }

                ClientRepository::ClientList::type ClientRepository::find(SNN subject) {
                    log::emit<AWDS>() << "Searching clients for subject (" << subject << ") ... ";
                    ClientList::type result = ClientList::Create();

                    // look for registered subject
                    SubscriberMap::iterator sit = _snnmap.find(subject);

                    // subject not registered, return empty list
                    if (sit == _snnmap.end()) {
                        log::emit() << "no clients found." << log::endl;
                        return result;
                    }

                    SubscriberList::type cls = sit->second;

                    log::emit() << "found " << log::dec << cls->size() << " clients." << log::endl;

                    log::emit<AWDS>() << "Checking clients ... " << log::endl;

                    int bad_subscribers = 0;

                    // attributes of publischer
                    Attributes::type pubA = _snnAttribs[subject];

                    // add  clients to result list
                    for (SubscriberList::iterator it = cls->begin(); it != cls->end(); it++) {
                        // cA are the actual client attributes
                        Attributes::type subA, cA = it->attribs;

                        // find the attributes of subsciber
                        for (SubscriberList::iterator it2 = _clients->begin(); it2 != _clients->end(); it2++)
                            if (it2->client == it->client){
                                subA = it2->attribs;
                                break; // we dont need to loop the rest
                            }

                        // check age and attributes of clients
                        if ((it->client->elapsed() <= _maxAge) && (cA <= subA) && (subA <= pubA))
                            // add good client
                            result->add(it->client);
                        else
                            bad_subscribers++;
                    }

                    log::emit<AWDS>() << log::dec << "Found " << bad_subscribers << " bad subscribers." << log::endl;

                    return result;
                }

                void ClientRepository::remove(AWDSClient::type client) {
                    log::emit<AWDS>() << "Remove client: " << client << log::endl;
                    // unregister client from all subjects
                    unreg(client);

                    // remove client from client list
                    for (SubscriberList::iterator it = _clients->begin(); it != _clients->end(); it++)
                        if (it->client == client){
                            _clients->erase(it);
                            break; // we don't nee to loop the rest
                        }
                }

                void ClientRepository::remove(SNN subject) {
                    log::emit<AWDS>() << "Remove subject: " << subject << log::endl;

                    SubscriberMap::iterator it = _snnmap.find(subject);

                    // subject registered, delete it
                    if (it != _snnmap.end()) {
                        it->second->clear(); // TODO: Do we have to do this?
                        _snnmap.erase(it);
                    }
                }

                void ClientRepository::reg(AWDSClient::type client, SNN subject, Attributes::type attribs) {
                    log::emit<AWDS>() << "Register client to subject: " << client << " | " << subject << log::endl;
                    // look for clients registered to given subject
                    SubscriberMap::iterator it = _snnmap.find(subject);

                    // subject not registered
                    if (it == _snnmap.end())
                        // TODO: Temporary workaround until publisher announcing subjects
                        reg(subject);

                    // add client to subject
                    _snnmap[subject]->add(Subscriber::Create(client, attribs));
                }

                void ClientRepository::reg(SNN subject, Attributes::type attribs) {
                    log::emit<AWDS>() << "Register subject: " << subject << log::endl;
                    // look for registered subject
                    SubscriberMap::iterator it = _snnmap.find(subject);

                    // subject not registered, register it
                    if (it == _snnmap.end()) {
                        _snnmap[subject] = SubscriberList::Create();
                        _snnAttribs[subject] = attribs;
                    }
                }

                void ClientRepository::unreg(AWDSClient::type client) {
                    log::emit<AWDS>() << "Unregister client from subjects: " << client << log::endl;

                    // remove client from all subjects, loop over all subjects
                    for (SubscriberMap::iterator it = _snnmap.begin(); it != _snnmap.end(); it++) {

                        // loop over all clients registered to subject
                        for (SubscriberList::iterator it2 = it->second->begin(); it2 != it->second->end(); it2++) {

                            // client is registered to subject, so unregister it
                            if (it2->client == client){
                               it->second->erase(it2);
                               // client could not be registered more than on time
                               break;
                            }
                        }
                    }
                }

                void ClientRepository::maxAge(int age) {
                    _maxAge = age;
                }
            } /* awds */
        } /* nl */
    } /* mw */
} /* famouso */
