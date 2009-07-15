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
                    * \todo busy waiting in the bind_subject method at the
                    * moment but we have to block until the subject is bound.
                    * That could lead to a problem in case of using the client
                    * on a gateway node, because the waiting will be done
                    * within a asynchronous function call and it blocks the
                    * rest of the system (asio) till it is finished. For that
                    * purpose it needs a better solution.
                    * \n\n
                    * This is only a problem in case of using asio. A solution
                    * could be to create a thread within the
                    * famouso::mw::el::EventLayerMiddlewareStub that does the
                    * subscription and announcement of event channels that can
                    * lead to blocking in the bind function, however the
                    * asynchronous function call can return after the creation
                    * of the thread. The system is not stalled then. However,
                    * this leads to the next problem within the client, because
                    * the clients data structure are not thread safe at the
                    * moment.
                    * \n\n
                    * Furthermore, we have to find all places where event
                    * channels are created in order to workaround the described
                    * problem. -- famouso::mw::gwl::Gateway is also a
                    * candidate.
                    * \n\n
                    * A general workaround is always using a broker on
                    * gateways, and enabling synchronised brokers through
                    * replicating their data structures.
                    *
                    */
                    template < class CAN_Driver >
                    class Client {
                            struct BindSubjectInfo {
                                typename CAN_Driver::MOB mob;
                                uint16_t etag;
                                uint8_t round;
                                BindSubjectInfo () : etag(0), round(0){}
                            };

                            BindSubjectInfo * volatile _bsi;
                            public:

                            typedef typename CAN_Driver::MOB::IDType IDType;

                            Client(): _bsi(0) {}
                            /*! \brief Bind a Subject to an etag.
                             *
                             *  \param[in] sub the Subject that should be bound
                             *  \param[in] tx_node the node-id of the node that
                             *             requests a subject to etag binding
                             *  \param[in] canDriver the driver which is used to
                             *             deliver the request
                             *
                             *  \return    the bound etag
                             *
                             */
                            uint16_t bind_subject(const Subject &sub, uint16_t tx_node, CAN_Driver& canDriver) {
                                BindSubjectInfo bsi;
                                IDType *id = reinterpret_cast<IDType*>(&(bsi.mob.id()));
                                bsi.mob.len(8);
                                bsi.mob.extended();
                                for (uint8_t i = 0;i < bsi.mob.len();++i)
                                    bsi.mob.data()[i] = sub.tab()[i];

                                _bsi=&bsi;

                                id->prio(0xFD);
                                id->etag(famouso::mw::nl::CAN::detail::ETAGS::GET_ETAG);
                                id->tx_node(tx_node);
                                canDriver.transmit(bsi.mob);

                                while (_bsi);
                                return bsi.etag;

                            }

                            /*! \brief handle a Subject bind request.
                             *
                             *  \param[in] mob the CAN message that contains the
                             *             Subject that should be bound
                             *  \param[in] canDriver the driver which is used to
                             *             deliver answer to the request
                             *
                             *  \return true if it was a binding message else false.
                             */
                            bool handle_subject_bind_request(typename CAN_Driver::MOB &mob, CAN_Driver& canDriver) {
                                IDType *id = &mob.id();
                                if (id->etag() == famouso::mw::nl::CAN::detail::ETAGS::SUPPLY_ETAG_NEW_BP) {
                                    if (_bsi){
                                        // index describes which part of the subject is in the message
                                        uint8_t index=mob.data()[1]<<2;
                                        // check if we are in the correct round
                                        if ((_bsi->round<<2) == index) {
                                            typename CAN_Driver::MOB &request_mob=const_cast<typename CAN_Driver::MOB &>(_bsi->mob);
                                            // test if the got half subject matches with the subject of the request
                                            // if not then the answer of the broker was not for us.
                                            for (uint8_t count=0;count<4;++count)
                                                if (request_mob.data()[index+count] != mob.data()[4+count]) {
                                                    _bsi->round=0;
                                                    return true;
                                                }
                                            // if the half subject matches then process further
                                            _bsi->round++;
                                        }
                                        // Do we have two correct half subject matches?
                                        // if yes then the broker answer was for us and we
                                        // got the etag
                                        if (_bsi->round == 2 ) {
                                            // read the etag
                                            _bsi->etag = ((mob.data()[2] & 0x3f) << 8) + mob.data()[3];
                                            // signalise the blocked binding method that the request is fulfilled
                                            _bsi=NULL;
                                        }
                                    }
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

