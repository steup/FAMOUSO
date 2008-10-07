#ifndef __CCP_Broker_h__
#define __CCP_Broker_h__

#include "devices/nic/can/peak/PeakCAN.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/nl/can/canID.h"
#include "mw/common/UID.h"

namespace famouso {
  namespace mw {
    namespace nl {
      namespace CAN {

//
// damit der Algorithmus ordentlich funktioniert, ist eine
// Randbedingung, dass die UID = 0x0 von keinem Knoten
// verwendet wird.
//
template < class CAN_MOB, typename ID=famouso::mw::nl::CAN::detail::ID>
class CCP_Broker {
        struct constants {
            enum {
                Broker_tx_node = 0x7f,
                count = 0x7f
            };
        };

        UID knownNodes [constants::count];

        uint8_t ccp_stage;
        UID uid;

        uint8_t search_tx_node(UID &uid) {
            uint8_t freeplace = 0xff;
            for (uint8_t i = 0; i < constants::count; ++i) {
                if (knownNodes[i] == 0) {
                    freeplace = i;
                } else {
                    if ( knownNodes[i] == uid ) return i;
                }
            }
            if ( freeplace != 0xff )
                knownNodes[freeplace] = uid;
            return freeplace;
        }
    public:
        CCP_Broker() : ccp_stage(15), uid(0) {
            for ( uint8_t i = 0; i < constants::count; ++i ) {
                knownNodes[i].value = 0;
            }
        }

        bool handle_ccp_rsi(CAN_MOB &mob) {
//           if ( ccp_stage == 15 )
//                std::cout << "CAN Configuration Protocol request for nodeID" << std::hex << std::endl;

			ID *id = reinterpret_cast<ID*>(&mob.ID);
            // extrahiere zunaechst die wichtigsten Infos
            uint8_t nibble = id->ccp_nibble();
            uint8_t stage = id->ccp_stage();
            uint8_t tx_node = constants::Broker_tx_node;

//            std::cout << std::hex << "etag="	<< id->etag()
//                      << "\tnibble=" << static_cast<uint32_t>(nibble)
//                      << "\tstage=" <<  static_cast<uint32_t>(ccp_stage) << std::endl;

            // kommt die Nachricht von jemandem der im richtigen stage ist?
            if (!(ccp_stage == stage)) {
                std::cerr << "received ccp rsi for stage "
                          << static_cast<uint32_t>(stage)
                          << " but I am in stage "
                          << static_cast<uint32_t>(ccp_stage)
                          << " dropping it." << std::endl;
                return false;
            } else {

                // stage aktualisieren
                --ccp_stage;

                // Immer den StageZaehler im letzten nibble im letzten
                // Datenbyte mitschicken. Wenn stage == 0 dann wird es
                // durch den richtigen Inhalt spaeter ueberschrieben
                uid.tab[7] = (uid.tab[7] & 0xF0) | stage;

                // die UID zusammenfassen
                uid.tab[7-(stage>>1)] |= (stage & 0x1) ? (nibble << 4) : nibble;

                // 0xFD is non-real-time
                // \todo RealTimeClasses for the CAN-Bus needs to be defined somewhere
                id->prio(0xFD);

                id->etag(famouso::mw::nl::CAN::ETAGS::CCP_SSI);

                mob.LEN = 8;

                // kopieren des uid in die Nachricht um auf ClientSeite die Arbitrierung
                // durchfuehren zu koennen.
                for (uint8_t i = 0; i < 8; ++i) {
                    mob.DATA[i] = uid.tab[i];
                }

                // wenn wir im letzten Stage sind, weisen wir eine KnotenID zu
                if (!stage) {
                    // generiere KnotenID, kein freier darf nicht vorkommen, weil dann
                    // der knoten keine tx_node bekommt
                    if ( (tx_node = search_tx_node(uid)) > constants::Broker_tx_node) {
                        std::cerr << "more than " << constants::Broker_tx_node
                                  << " nodes configured that means something went wrong" << std::endl;
                        return false;
                    } else {
                        std::cout << std::hex << "CCP\t\t -- UID 0x" << uid.value
                                  << " [" << reinterpret_cast<char*>(&uid.value) << "] "
                                  << "-> tx_node [0x"
                                  << static_cast<uint32_t>(tx_node) << "]" << std::endl;
                    }
                    ccp_stage = 15;
                    uid = 0;
                }
                // tx_node ist entweder die des Brokers, wenn noch nicht stage 0 und
                // sonst die txnode des neuen Knoten
                id->tx_node(tx_node);
            }
            return true;
        }
};

      } /* namespace CAN */
    } /* namespace nl */
  } /* namespace mw */
} /* namespace famouso */


#endif