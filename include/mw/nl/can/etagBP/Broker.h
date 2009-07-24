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

#ifndef __etagBP_Broker_h__
#define __etagBP_Broker_h__

#include "mw/nl/can/constants.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/common/Subject.h"
#include "util/endianness.h"

namespace famouso {
    namespace mw {
        namespace nl {
            namespace CAN {
                namespace etagBP {
                    using namespace famouso::mw::nl::CAN::detail;

                    /*! \brief ETAG binding protocol the broker part supports
                     *         both the old and new one
                     *
                     * The protocol binds a famouso::mw::Subject to a short
                     * network specific representation. Within this protocol,
                     * the mapping is from 64Bit to 14Bit, meaning, we are
                     * unable to map all subjects. Thus, it is impossible to
                     * communicate with more than 14Bit different subject
                     * within a %CAN environment. This seems to be a
                     * limitation, but normally a typical application
                     * environment uses only few different subjects.
                     *
                     * The client part is in famouso::mw::nl::CAN::etagBP::Client.
                     */
                    template < class CAN_Driver>
                    class Broker {

                            Subject etags[constants::etagBP::count];

                            /*! \brief Bind a Subject to an etag.
                             *
                             *  \param[in] sub the Subject that should be bound
                             *  \return the bound etag in case of success else 0
                             */
                            uint16_t bind_subject_to_etag(const Subject &sub) {
                                uint16_t etag = constants::etagBP::count;
                                while (etag > constants::etagBP::reserved) {
                                    --etag;
                                    if ((etags[etag] == UID()) || (etags[etag] == sub)) {
                                        etags[etag] = sub;
                                        std::cout << "Supply etag\t -- Subject [0x" << std::hex << sub.value() << "]"
                                        << " -> etag [0x" << etag << "]" << std::endl;
                                        return etag;
                                    }
                                }
                                std::cout << "no free etags -- that should never be happen" << std::endl;
                                return 0;
                            }

                        public:
                            typedef typename CAN_Driver::MOB::IDType IDType;

                            /*! \brief Bind a Subject to an etag.
                             *
                             *  \param[in] sub the Subject that should be bound
                             *  \param[in] tx_node the node-id of the node that
                             *             requests a subject to etag binding
                             *  \param[in] canDriver the driver which is used to
                             *             deliver the request
                             *
                             *  \return the bound etag in case of success else 0
                             */
                            uint16_t bind_subject(const Subject &sub, uint16_t tx_node, CAN_Driver& canDriver) {
                                return bind_subject_to_etag(sub);
                            }

                            /*! \brief handle a Subject bind request.
                             *
                             *  \param[in] mob the CAN message that contains the
                             *             Subject that should be bound
                             *  \param[in] canDriver the driver which is used to
                             *             deliver answer to the request
                             *
                             *  \return true if it a binding was succeeded and false
                             *          in case the message wasn't a binding request
                             */
                            bool handle_subject_bind_request(typename CAN_Driver::MOB &mob, CAN_Driver& canDriver) {
                                IDType *id = &mob.id();
                                Subject sub(mob.data());
                                if (id->etag() == famouso::mw::nl::CAN::detail::ETAGS::GET_ETAG) {
                                    uint16_t etag = bind_subject_to_etag(sub);
                                    mob.len(4);
                                    mob.data()[0] = id->tx_node();
                                    mob.data()[1] = 0x3;
                                    mob.data()[2] = etag >> 8;
                                    mob.data()[3] = static_cast<uint8_t>(etag & 0xff);
                                    id->prio(0xFD);
                                    id->etag(famouso::mw::nl::CAN::detail::ETAGS::SUPPLY_ETAG);
                                    id->tx_node(constants::Broker_tx_node);
                                    canDriver.transmit(mob);

                                    mob.len(8);
                                    mob.data()[0] = id->tx_node();
                                    mob.data()[1] = 0x0;
                                    mob.data()[2] = etag >> 8;
                                    mob.data()[3] = static_cast<uint8_t>(etag & 0xff);
                                    mob.data()[4] = sub.tab()[0];
                                    mob.data()[5] = sub.tab()[1];
                                    mob.data()[6] = sub.tab()[2];
                                    mob.data()[7] = sub.tab()[3];
                                    id->etag(famouso::mw::nl::CAN::detail::ETAGS::SUPPLY_ETAG_NEW_BP);
                                    id->tx_node(constants::Broker_tx_node);
                                    canDriver.transmit(mob);

                                    mob.data()[1] = 0x1;
                                    mob.data()[4] = sub.tab()[4];
                                    mob.data()[5] = sub.tab()[5];
                                    mob.data()[6] = sub.tab()[6];
                                    mob.data()[7] = sub.tab()[7];
                                    canDriver.transmit(mob);

                                    return true;
                                }
                                return false;
                            }
                    };

                }
            } /* namespace CAN */
        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */

#endif

