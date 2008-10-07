#ifndef __ccp_h__
#define __ccp_h__

#include "devices/nic/can/peak/PeakCAN.h"
#include "mw/nl/can/constants.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/nl/can/canID.h"
#include "mw/common/UID.h"


namespace famouso {
  namespace mw {
    namespace nl {
      namespace CAN {
namespace ccp {
		enum {
			CCP_RUNNING=0,	/*!< Configuration is running */
			CCP_COMPLETE,	/*!< Configuration is complete */
			CCP_WAIT		/*!< Configuration is pending */
		};

//
// damit der Algorithmus ordentlich funktioniert, ist eine
// Randbedingung, dass die UID = 0x0 von keinem Knoten
// verwendet wird.
//
template < class CAN_MOB, typename ID=famouso::mw::nl::CAN::detail::ID>
class Broker {

        UID knownNodes [constants::ccp::count];
        uint8_t ccp_stage;
        UID uid;

        uint8_t search_tx_node(UID &uid) {
            uint8_t freeplace = 0xff;
            for (uint8_t i = 0; i < constants::ccp::count; ++i) {
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
        Broker() : ccp_stage(15), uid(0) {
            for ( uint8_t i = 0; i < constants::ccp::count; ++i ) {
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

template < class CAN_Driver, typename ID=famouso::mw::nl::CAN::detail::ID>
class Client {
void debug(char* s){
		std::cout<<s;
	}
uint8_t compareUID(uint8_t *msg, uint8_t* uid_str, uint8_t stage)
{
	register int8_t count;

	for (count=constants::ccp::ccp_stages;count>=stage;--count) {
		register uint8_t uidPart = (constants::ccp::ccp_stages-count) >> 1;
		register uint8_t uidNibble = (count%2) ?  (uid_str[uidPart] >> 4) : (uid_str[uidPart] &0xf);
		register uint8_t msgNibble = (count%2) ?  (msg[uidPart] >> 4) : (msg[uidPart] &0xf);
		if (uidNibble!=msgNibble) {
//			if (stage != 0 ) debug(" UIDs nicht identisch. folglich konfiguriert anderer Knoten parallel.\n");
			return 0;
		}
	}
	return 1;
}

 public:

uint8_t ccp_run(const char* uid, CAN_Driver& canDriver)
{
	uint8_t *uid_str= (uint8_t*)uid;
	typename CAN_Driver::MOB	msg;
	// \todo hier besteht noch eine Abhaengigkeit zu Peak
	//		 sollte in der kommenden Version ueber ein allgemeines
	//		 CAN-MSG-Format geloesst werden.
	msg.MSGTYPE=MSGTYPE_EXTENDED;

	ID *id = reinterpret_cast<ID*>(&msg.ID);
	uint8_t ccp_status;
	uint8_t ccp_stage;
	uint8_t tx_node;
	int8_t stage;

	uint8_t zeros[8]={0};

	ccp_status = CCP_RUNNING;
	ccp_stage = 0;

	// Anzahl der durchzufuehrenden Stages
	stage=constants::ccp::ccp_stages;
	do {
		// erzeuge die entsprechende ID
		register uint8_t uidPart=(constants::ccp::ccp_stages-stage) >> 1;
	    register uint8_t uidNibble = (stage%2) ? (uid_str[uidPart] >> 4) :
						(uid_str[uidPart] &0xf);

        // 0xFD is non-real-time
        // \todo RealTimeClasses for the CAN-Bus needs to be defined somewhere
        id->prio(0xFD);
		id->ccp_stage(stage);
		id->ccp_nibble(uidNibble);

//		std::cout<<"Stage " << (uint32_t)id->ccp_stage() << std::endl;
//		std::cout<<"Nibble " << (uint32_t)id->ccp_nibble() << std::endl;
        id->etag(famouso::mw::nl::CAN::ETAGS::CCP_RSI);

	// \todo hier besteht noch eine Abhaengigkeit zu Peak
	//		 sollte in der kommenden Version ueber ein allgemeines
	//		 CAN-MSG-Format geloesst werden.
		msg.LEN = 0;

		// Nachricht senden
		canDriver.send(msg);

		ccp_status = CCP_WAIT;

		do {
			// und auf Antwort warten
			do {
				canDriver.receive_blocking(&msg);
				// Ist die Nachricht ein Konfigurationsantwort
			} while ( id->etag() != famouso::mw::nl::CAN::ETAGS::CCP_SSI );

			// extrahiere die KnotenID
			tx_node=id->tx_node();
			// ja, also laufen wir erstmal weiter
			ccp_status=CCP_RUNNING;
			// ist dies eine Brokernachricht mit Stagezaehler im letzten Datenbyte der Nachricht
			// wird angezeigt durch die knotenID == BrokerID == 0x7f
			if ( tx_node == constants::Broker_tx_node ) {
				// ist ein knoten vielleicht ausgestiegen, dann generiert der Broker
				// eine Nachricht mit leerem Inhalt, um unselektierte Knoten zur
				// Neukonfiguration zu bewegen
				if (compareUID(msg.DATA, zeros, 0))	{
					stage = constants::ccp::ccp_stages;
				} else {
					if ((stage == (msg.DATA[7] & 0xf)) && compareUID(msg.DATA,uid_str,stage) ) {
						// gehe in den naechsten Zyklus
						--stage;
					} else {
						ccp_status=CCP_WAIT;
					}
				}

			} else {
				// die BrokerNachricht enthaelt keinen Stagezaehler mehr, dafuer aber eine
				// konfigurierte KnotenID, weil sich mindestens ein Knoten in Stage 0 befindet
				// dies koennte ich sein. Einfach mal die uid mit den Daten vergleichen

				// untersuche Nachrichteninhalt auf Gleichheit mit eigener ID
				// wenn ja, extrahierte txnode ist meine
				// verlasse beide Schleifen da stage==0 und ccp_status==CCP_RUNNING
				if (compareUID(msg.DATA, uid_str, 0)) {
					ccp_stage=CCP_COMPLETE;
//					debug("Knoten ID ist meine\n");
				} else {
					// wenn nein einfach von vorn
					// und gehe in die aeussere Schleife weil ccp_status==CCP_RUNNING
					// und setze stage wieder auf den initialen Wert
//					debug("Knoten ID ist nicht meine\n");
					stage = constants::ccp::ccp_stages;
				}
			}
		} while( (ccp_status!=CCP_RUNNING) );
	} while ( (ccp_stage !=CCP_COMPLETE) );

	// Konfiguration erfolgreich und KnotenID erhalten
	return tx_node;
}

};
}
      } /* namespace CAN */
    } /* namespace nl */
  } /* namespace mw */
} /* namespace famouso */


#endif
