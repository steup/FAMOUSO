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

#include <list>
#include <map>
#include "boost/noncopyable.hpp"
#include "boost/shared_ptr.hpp"
#include "mw/common/Subject.h"
#include "mw/nl/awds/AWDSClient.h"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace awds {

                typedef boost::shared_ptr<AWDSClient> Client_sp;

                typedef std::list<Client_sp> ClientList;

                typedef boost::shared_ptr<ClientList> ClientList_sp;

                typedef boost::shared_ptr<famouso::mw::Subject> Subject_sp;

                class ClientRepository: boost::noncopyable {

                public:
                    static ClientRepository& getInstance();

                    Client_sp find(MAC mac);
                    ClientList_sp find(Subject_sp subject);
                    void remove(Client_sp client);
                    void remove(Subject_sp);

                private:
                    typedef std::map<Subject_sp, ClientList_sp> SNNClientMap;
                    typedef std::map<MAC, Client_sp> ClientMap;

                    SNNClientMap _snnmap;
                    ClientList _clients;
                };
            } /* awds */
        } /* nl */
    } /* mw */
} /* famouso */
#define __ClientRepository_hpp__
#endif /* __ClientRepository_hpp__ */
