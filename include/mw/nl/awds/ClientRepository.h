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
#ifndef __ClientRepository_hpp__
#define __ClientRepository_hpp__

#include <list>
#include <map>
#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"
#include "mw/common/Subject.h"
#include "MAC.h"
#include "AWDSClient.h"

namespace famouso {
	namespace mw {
		namespace nl {
			namespace _awds {

				/*! \brief A list of clients at awds.
				 */
				typedef std::list<awds::Client_sp> ClientList;

				/*! \brief A famouso Subject.
				 */
				typedef famouso::mw::Subject SNN;

				/*! \brief A list of clients at awds.
				 */
				typedef boost::shared_ptr<ClientList> ClientList_sp;

				/*! \brief An iterator for the clients list.
				 */
				typedef ClientList::iterator ClientList_sp_iterator;

				/*! \brief A map to assign clients to subjects.
				 */
				typedef std::map<SNN, ClientList_sp> SNNClientMap;

				/*! \brief A client repository for holding awds clients and register clients to subjects.
				 */
				class ClientRepository: boost::noncopyable {

					public:
					/*! \brief Get the only instance of the clientsrepository.
					 *
					 *  \return A reference to the only instance of the repository.
					 */
						static ClientRepository& getInstance();

						/*! \brief Find a client by a given MAC address.
						 *         If the client doesn't exists, it will be created.
						 *
						 * \param mac The MAC address to find the client for.
						 * \return The client corresponding to the MAC address.
						 */
						awds::Client_sp find(MAC mac);

						/*! \brief Find all clients registered to the given Subject.
						 *
						 *  \param subject The subject to find clients for.
						 *  \return A list with clients registered for the given subject.
						 *         The list can be empty, if no client is registered.
						 */
						ClientList_sp find(SNN subject);

						/*! \brief Remove a client from the repository.
						 *         It will be removed from all subjects too.
						 *
						 *  \param client The client to remove.
						 */
						void remove(awds::Client_sp client);

						/*! \brief Remove a subject from the repository.
						 *         Clients registered to the given subject will
						 *         be unregistered from it.
						 *
						 *  \param subject The subject to remove.
						 */
						void remove(SNN subject);

						/*! \brief Register a client to a subject.
						 *         If the subject isn't registered, this function
						 *         does nothing.
						 *
						 *  \param client The client to register.
						 *  \param subject The subject to register.
						 */
						void reg(awds::Client_sp client, SNN subject);

						/*! \brief Register a subject to the repository.
						 *
						 *  \param subject The subject to register.
						 */
						void reg(SNN subject);

						/*! \brief Unregister the given client from all known subjects.
						 *
						 *  \param client The client to unregister.
						 */
						void unreg(awds::Client_sp client);

					private:
						SNNClientMap _snnmap;
						ClientList _clients;
				};

			} /* _awds */

			namespace awds {

				/*! \brief A list of clients at awds.
				 */
				typedef _awds::ClientList_sp ClientList_sp;

				/*! \brief An iterator for the clients list.
				 */
				typedef _awds::ClientList_sp_iterator ClientList_sp_iterator;

				/*! \brief A client repository for holding awds clients and register clients to subjects.
				 */
				typedef _awds::ClientRepository ClientRepository;
			} /* awds */
		} /* nl */
	} /* mw */
} /* famouso */
#endif /* __ClientRepository_hpp__ */
