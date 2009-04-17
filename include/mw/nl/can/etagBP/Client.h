/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __etagBP_Client_h__
#define __etagBP_Client_h__

#include "mw/nl/can/constants.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/common/Subject.h"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace CAN {
                namespace etagBP {

                    /*!
                    * \brief new ETAG binding protocol
                    *
                    * \todo busy waiting in the bind_subject method at the moment
                    *   but we have to block until the subject is bound. That
                    *   could lead to a problem in case of using the client on
                    *   a gateway node, because the waiting will be done within
                    *   a asynchrone function call and it blocks the rest of the
                    *   system (asio) till it is finished. For that purpose it
                    *   needs a better solution.
                    *
                    * \todo at the moment the received broker answer is not checked
                    *   whether the etag corresponds the the subject
                    */
                    template < class CAN_Driver >
                    class Client {
                            volatile bool passed;
                            uint16_t etag;
                        public:

                            typedef typename CAN_Driver::MOB::IDType IDType;
                            Client(): passed(false), etag(0) {}
                            /*! \brief Bind a Subject to an etag.
                             *
                             *  \param[in] sub the Subject that should be bound
                             *  \return the bound etag in case of success else 0
                             *
                             */
                            uint16_t bind_subject(const Subject &sub, uint16_t tx_node, CAN_Driver& canDriver) {
                                typename CAN_Driver::MOB mob;
                                IDType *id = reinterpret_cast<IDType*>(&mob.id());
                                mob.len(8);
                                for (uint8_t i = 0;i < mob.len();++i)
                                    mob.data()[i] = sub.tab()[i];

                                id->prio(0xFD);
                                id->etag(famouso::mw::nl::CAN::ETAGS::GET_ETAG);
                                id->tx_node(tx_node);
                                canDriver.send(mob);

                                while (!passed);
                                passed = false;
                                return etag;

                            }

                            /*! \brief handle a Subject bind request.
                             *
                             *  \param[in] mob the CAN message that contains the
                             *             Subject that should be bound
                             *  \param[in] canDriver the driver which is used to
                             *             deliver answer to the request
                             *
                             *  \return always false, because the client can not handle
                             *          such requests. This allows the compiler further
                             *          optimizations and in the best case the code is
                             *          complete removed.
                             */
                            bool handle_subject_bind_request(typename CAN_Driver::MOB &mob, CAN_Driver& canDriver) {
                                IDType *id = &mob.id();
                                if (id->etag() == famouso::mw::nl::CAN::ETAGS::SUPPLY_ETAG_NEW_BP) {
                                    etag = ((mob.data()[2] & 0x3f) << 8) + mob.data()[3];
                                    passed = true;
                                    return true;
                                } else {
                                    return false;
                                }
                            }
                    };

                }
            } /* namespace CAN */
        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */

#endif

