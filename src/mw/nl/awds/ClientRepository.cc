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

#include "mw/nl/awds/ClientRepository.h"
#include "mw/nl/awds/AWDSClient.h"

namespace famouso {
	namespace mw {
		namespace nl {
			namespace _awds {
				using namespace awds;

				ClientRepository & ClientRepository::getInstance() {
					static ClientRepository instance;
					return instance;
				}

				Client_sp ClientRepository::find(MAC mac) {
					// look for a client with specified mac
					for (ClientList::iterator it = _clients.begin(); it != _clients.end(); it++) {
						if (mac == (*it)->mac())
							return *it;
					}

					// client not found, create a new and register
					Client_sp result = Client_sp(new AWDSClient(mac));
					_clients.push_back(result);
					return result;
				}

				ClientList_sp ClientRepository::find(SNN subject) {
					ClientList_sp result = ClientList_sp(new ClientList());

					// look for clients registered to given subject
					SNNClientMap::iterator it = _snnmap.find(subject);

					// subject not registered, return empty list
					if (it == _snnmap.end())
						return result;

					// subject found, get it
					ClientList_sp cls = (*it).second;

					// add all clients to result list
					for (ClientList_sp_iterator it2 = cls->begin(); it2 != cls->end(); it2++)
						result->push_back(*it2);

					return result;
				}

				void ClientRepository::remove(Client_sp client) {
					// unregister client from all subjects
					unreg(client);

					// remove client from client list
					_clients.remove(client);
				}

				void ClientRepository::remove(SNN subject) {
					SNNClientMap::iterator it = _snnmap.find(subject);

					// subject registered, delete it
					if (it != _snnmap.end()) {
						(*it).second->clear();
						_snnmap.erase(it);
					}
				}

				void ClientRepository::reg(Client_sp client, SNN subject) {
					// look for clients registered to given subject
					SNNClientMap::iterator it = _snnmap.find(subject);

					// subject registered, add client to subject
					if (it != _snnmap.end())
						(*it).second->push_back(client);
				}

				void ClientRepository::reg(SNN subject) {
					// look for registered subject
					SNNClientMap::iterator it = _snnmap.find(subject);

					// subject not registered, register it
					if (it == _snnmap.end())
						_snnmap[subject] = ClientList_sp(new ClientList());
				}

				void ClientRepository::unreg(Client_sp client) {
					// remove client from all subjects
					for (SNNClientMap::iterator it = _snnmap.begin(); it != _snnmap.end(); it++)
						(*it).second->remove(client);
				}
			} /* _awds */
		} /* nl */
	} /* mw */
} /* famouso */
