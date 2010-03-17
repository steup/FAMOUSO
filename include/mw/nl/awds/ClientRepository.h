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
#include "mw/nl/awds/MAC.h"
#include "mw/nl/awds/AWDSClient.h"
#include "mw/nl/awds/Attributes.h"
#include "mw/nl/awds/lists.h"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {

                /*! \brief A client repository for holding awds clients and register clients to subjects.
                 */
                class ClientRepository: boost::noncopyable {

                    public:

                        /*! \brief A client repository for holding awds clients and register clients to subjects.
                         *
                         */
                        typedef ClientRepository type;

                        typedef List<AWDSClient::type> ClientList;

                        /*! \brief A famouso Subject.
                         */
                        typedef famouso::mw::Subject SNN;

                    private:

                        class Subscriber {
                            public:
                                static Subscriber Create(AWDSClient::type c, Attributes::type a) {
                                    Subscriber res;
                                    res.client = c;
                                    res.attribs = a;
                                    return res;
                                }

                                AWDSClient::type client;
                                Attributes::type attribs;

                        };

                        typedef List<Subscriber> SubscriberList;

                        /*! \brief A map to assign clients to subjects.
                         */
                        typedef std::map<SNN, SubscriberList::type> SubscriberMap;

                        /*! \brief A map to assign attributes to subjects.
                         */
                        typedef std::map<SNN, Attributes::type> PublisherMap;

                        ClientRepository();

                    public:

                        /*! \brief Get the only instance of the clientsrepository.
                         *
                         *  \return A reference to the only instance of the repository.
                         */
                        static type& getInstance();

                        /*! \brief Find a client by a given MAC address.
                         *         If the client doesn't exists, it will be created.
                         *
                         * \param mac The MAC address to find the client for.
                         * \return The client corresponding to the MAC address.
                         */
                        AWDSClient::type find(MAC mac);

                        /*! \brief Find all clients registered to the given Subject.
                         *
                         *  \param subject The subject to find clients for.
                         *  \return A list with clients registered for the given subject.
                         *         The list can be empty, if no client is registered.
                         */
                        ClientList::type find(SNN subject);

                        /*! \brief Remove a client from the repository.
                         *         It will be removed from all subjects too.
                         *
                         *  \param client The client to remove.
                         */
                        void remove(AWDSClient::type client);

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
                        void reg(AWDSClient::type client, SNN subject, Attributes::type attribs = Attributes::create());

                        /*! \brief Register a subject to the repository.
                         *
                         *  \param subject The subject to register.
                         */
                        void reg(SNN subject, Attributes::type attribs = Attributes::create());

                        /*! \brief Unregister the given client from all known subjects.
                         *
                         *  \param client The client to unregister.
                         */
                        void unreg(AWDSClient::type client);

                        /*! \brief The maximum time when a client has to be resubscribe.
                         *
                         *         The default value is 70 seconds.
                         *
                         *  \param age The time in seconds.
                         */
                        void maxAge(int age);


                        /*! \brief Update attribute of given client.
                         *
                         *  \param client The client to update.
                         *  \param attribs The new attributes to set.
                         */
                        void update(AWDSClient::type client, Attributes::type attribs = Attributes::create());

                    private:
                        SubscriberMap _snnmap;
                        SubscriberList::type _clients;
                        PublisherMap _snnAttribs;
                        int _maxAge;
                };

            } /* awds */
        } /* nl */
    } /* mw */
} /* famouso */
#endif /* __ClientRepository_hpp__ */
