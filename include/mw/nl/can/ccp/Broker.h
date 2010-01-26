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

#ifndef __ccp_Broker_h__
#define __ccp_Broker_h__

#include "mw/nl/can/constants.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/common/UID.h"


namespace famouso {
    namespace mw {
        namespace nl {
            namespace CAN {
                namespace ccp {
                    using namespace famouso::mw::nl::CAN::detail;

                    //
                    // damit der Algorithmus ordentlich funktioniert, ist eine
                    // Randbedingung, dass die UID = 0x0 von keinem Knoten
                    // verwendet wird.
                    //
                    template < class CAN_Driver >
                    class Broker {

                            typedef typename CAN_Driver::MOB::IDType IDType;

                            UID knownNodes [constants::ccp::count];
                            uint8_t ccp_stage;
                            UID uid;

                            uint8_t search_tx_node(const UID &uid) {
                                uint8_t freeplace = 0xff;
                                for (uint8_t i = 0; i < constants::ccp::count; ++i) {
                                    if (knownNodes[i] == UID()) {
                                        freeplace = i;
                                    } else {
                                        if (knownNodes[i] == uid) return i;
                                    }
                                }
                                if (freeplace != 0xff)
                                    knownNodes[freeplace] = uid;
                                return freeplace;
                            }

                            bool handle_ccp_rsi(typename CAN_Driver::MOB &mob) {
                                IDType *id = &mob.id();
                                // extrahiere zunaechst die wichtigsten Infos
                                uint8_t nibble = id->ccp_nibble();
                                uint8_t stage = id->ccp_stage();
                                uint8_t tx_node = constants::Broker_tx_node;

                                // kommt die Nachricht von jemandem der im richtigen stage ist?
                                if (!(ccp_stage == stage)) {
                                    ::logging::log::emit< ::logging::Error>() << "received ccp rsi for stage "
                                        << static_cast<uint32_t>(stage)
                                        << " but I am in stage "
                                        << static_cast<uint32_t>(ccp_stage)
                                        << " dropping it." << ::logging::log::endl;
                                        return false;
                                } else {
                                    // stage aktualisieren
                                    --ccp_stage;
                                    // Immer den StageZaehler im letzten nibble im letzten
                                    // Datenbyte mitschicken. Wenn stage == 0 dann wird es
                                    // durch den richtigen Inhalt spaeter ueberschrieben
                                    uid.tab()[7] = (uid.tab()[7] & 0xF0) | stage;
                                    // die UID zusammenfassen
                                    uid.tab()[7-(stage>>1)] |= (stage & 0x1) ? (nibble << 4) : nibble;
                                    // 0xFD is non-real-time
                                    // \todo RealTimeClasses for the CAN-Bus needs to be defined somewhere
                                    id->prio(0xFD);
                                    id->etag(famouso::mw::nl::CAN::detail::ETAGS::CCP_SSI);
                                    mob.len(8);
                                    // kopieren des uid in die Nachricht um auf ClientSeite die Arbitrierung
                                    // durchfuehren zu koennen.
                                    for (uint8_t i = 0; i < 8; ++i) {
                                        mob.data()[i] = uid.tab()[i];
                                    }
                                    // wenn wir im letzten Stage sind, weisen wir eine KnotenID zu
                                    if (!stage) {
                                        // generiere KnotenID, kein freier darf nicht vorkommen, weil dann
                                        // der knoten keine tx_node bekommt
                                        if ((tx_node = search_tx_node(uid)) > constants::Broker_tx_node) {
                                            ::logging::log::emit< ::logging::Error>() << "more than "
                                                << static_cast<short>(constants::Broker_tx_node)
                                                << " nodes configured that means something went wrong"
                                                << ::logging::log::endl;
                                            return false;
                                        } else {
                                            ::logging::log::emit() << "CCP" << ::logging::log::tab
                                                << ::logging::log::tab << ::logging::log::tab << "-- NodeID  [";
                                            for (uint8_t i = 0;i < 8;++i) {
                                                uint8_t c = uid.tab()[i];
                                                if ((c < 32) || (c > 126)) c = 32;   // only printable characters
                                                ::logging::log::emit() << c ;
                                            }
                                            ::logging::log::emit() << "] -> " << ::logging::log::hex
                                                << "tx_node [" << static_cast<uint16_t>(tx_node) << "] "
                                                << uid.value() << ::logging::log::endl;
                                        }
                                        ccp_stage = 15;
                                        uid = UID();
                                    }
                                    // tx_node ist entweder die des Brokers, wenn noch nicht stage 0 und
                                    // sonst die txnode des neuen Knoten
                                    id->tx_node(tx_node);
                                }
                                return true;
                            }
                        public:
                            Broker() : ccp_stage(15), uid() {}

                            bool handle_ccp_configure_request(typename CAN_Driver::MOB &mob, CAN_Driver& canDriver) {
                                if (mob.id().etag() == famouso::mw::nl::CAN::detail::ETAGS::CCP_RSI) {
                                    if (handle_ccp_rsi(mob))
                                        canDriver.transmit(mob);
                                    return true;
                                } else {
                                    return false;
                                }
                            }

                            uint8_t ccp_configure_tx_node(const UID &uid, CAN_Driver& canDriver) {
                                return search_tx_node(uid);
                            }
                    };

                } /* namespace ccp */
            } /* namespace CAN */
        } /* namespace nl */
    } /* namespace mw */
} /* namespace famouso */


#endif
