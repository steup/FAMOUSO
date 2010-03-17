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

                ClientRepository::ClientRepository(): _clients(ClientList::Create()){
                }

                AWDSClient::type ClientRepository::find(MAC mac) {
                    log::emit<AWDS>() << "Searching client ... ";
                    // look for a client with specified mac
                    for (ClientList::iterator it = _clients->begin(); it != _clients->end(); it++) {
                        if (mac == (*it)->mac()) {
                            log::emit() << "found: " << *it << log::endl;
                            return *it;
                        }
                    }

                    // client not found, create a new and register
                    AWDSClient::type result = AWDSClient::create(mac);
                    log::emit() << "not found: " << result << log::endl;
                    _clients->add(result);
                    return result;
                }

                ClientList::type ClientRepository::find(SNN subject) {
                    log::emit<AWDS>() << "Searching clients for subject (" << subject << ") ... ";
                    ClientList::type result = ClientList::Create();

                    // look for registered subject
                    SNNClientMap::iterator it = _snnmap.find(subject);

                    // subject not registered, return empty list
                    if (it == _snnmap.end()) {
                        log::emit() << "not found" << log::endl;
                        return result;
                    }

                    ClientList::type cls = (*it).second;

                    log::emit() << "found: " << log::dec << cls->size() << log::endl;

                    // add all clients to result list
                    for (ClientList::iterator it2 = cls->begin(); it2 != cls->end(); it2++)
                        result->add(*it2);

                    return result;
                }

                void ClientRepository::remove(AWDSClient::type client) {
                    log::emit<AWDS>() << "Remove client: " << client << log::endl;
                    // unregister client from all subjects
                    unreg(client);

                    // remove client from client list
                    _clients->remove(client);
                }

                void ClientRepository::remove(SNN subject) {
                    log::emit<AWDS>() << "Remove subject: " << subject << log::endl;

                    SNNClientMap::iterator it = _snnmap.find(subject);

                    // subject registered, delete it
                    if (it != _snnmap.end()) {
                        (*it).second->clear();
                        _snnmap.erase(it);
                    }
                }

                void ClientRepository::reg(AWDSClient::type client, SNN subject) {
                    log::emit<AWDS>() << "Register client to subject: " << client << " | " << subject << log::endl;
                    // look for clients registered to given subject
                    SNNClientMap::iterator it = _snnmap.find(subject);

                    // subject registered,
                    if (it == _snnmap.end())
                        // Temporary workaround until publisher announcing subjects
                        _snnmap[subject] = ClientList::Create();

                    // add client to subject
                    _snnmap[subject]->add(client);
                }

                void ClientRepository::reg(SNN subject) {
                    log::emit<AWDS>() << "Register subject: " << subject << log::endl;
                    // look for registered subject
                    SNNClientMap::iterator it = _snnmap.find(subject);

                    // subject not registered, register it
                    if (it == _snnmap.end())
                        _snnmap[subject] = ClientList::Create();
                }

                void ClientRepository::unreg(AWDSClient::type client) {
                    log::emit<AWDS>() << "Unregister client from subjects: " << client << log::endl;
                    // remove client from all subjects
                    for (SNNClientMap::iterator it = _snnmap.begin(); it != _snnmap.end(); it++)
                        (*it).second->remove(client);
                }
            } /* awds */
        } /* nl */
    } /* mw */
} /* famouso */
