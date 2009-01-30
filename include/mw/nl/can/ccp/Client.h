#ifndef __ccp_Client_h__
#define __ccp_Client_h__

#include "mw/nl/can/constants.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/common/UID.h"


namespace famouso {
namespace mw {
namespace nl {
namespace CAN {
namespace ccp {
enum {
    CCP_RUNNING = 0,    /*!< Configuration is running */
    CCP_COMPLETE,       /*!< Configuration is complete */
    CCP_WAIT            /*!< Configuration is pending */
};

//
// damit der Algorithmus ordentlich funktioniert, ist eine
// Randbedingung, dass die UID = 0x0 von keinem Knoten
// verwendet wird.
//

template < class CAN_Driver >
class Client {
        void debug(char* s) {
            std::cout << s;
        }
        uint8_t compareUID(uint8_t *msg, uint8_t* uid_str, uint8_t stage) {
            register int8_t count;

            for (count = constants::ccp::ccp_stages;count >= stage;--count) {
                register uint8_t uidPart = (constants::ccp::ccp_stages - count) >> 1;
                register uint8_t uidNibble = (count % 2) ?  (uid_str[uidPart] >> 4) : (uid_str[uidPart] & 0xf);
                register uint8_t msgNibble = (count % 2) ?  (msg[uidPart] >> 4) : (msg[uidPart] & 0xf);
                if (uidNibble != msgNibble) {
                    return 0;
                }
            }
            return 1;
        }

    public:

        typedef typename CAN_Driver::MOB::IDType IDType;

        uint8_t ccp_configure_tx_node(const char* uid, CAN_Driver& canDriver) {
            uint8_t *uid_str = (uint8_t*)uid;
            typename CAN_Driver::MOB	msg;
            msg.extended();

            IDType *id = &msg.id();
            uint8_t ccp_status;
            uint8_t ccp_stage;
            uint8_t tx_node;
            int8_t stage;

            uint8_t zeros[8] = {0};

            ccp_status = CCP_RUNNING;
            ccp_stage = 0;

            // Anzahl der durchzufuehrenden Stages
            stage = constants::ccp::ccp_stages;
            do {
                // erzeuge die entsprechende ID
                register uint8_t uidPart = (constants::ccp::ccp_stages - stage) >> 1;
                register uint8_t uidNibble = (stage % 2) ? (uid_str[uidPart] >> 4) :
                                             (uid_str[uidPart] & 0xf);

                // 0xFD is non-real-time
                /*! \todo RealTimeClasses for the CAN-Bus needs to be defined somewhere */
                id->prio(0xFD);
                id->ccp_stage(stage);
                id->ccp_nibble(uidNibble);

                id->etag(famouso::mw::nl::CAN::ETAGS::CCP_RSI);

                msg.len(0);

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
                    tx_node = id->tx_node();
                    // ja, also laufen wir erstmal weiter
                    ccp_status = CCP_RUNNING;
                    // ist dies eine Brokernachricht mit Stagezaehler im letzten Datenbyte der Nachricht
                    // wird angezeigt durch die knotenID == BrokerID == 0x7f
                    if ( tx_node == constants::Broker_tx_node ) {
                        // ist ein knoten vielleicht ausgestiegen, dann generiert der Broker
                        // eine Nachricht mit leerem Inhalt, um unselektierte Knoten zur
                        // Neukonfiguration zu bewegen
                        if (compareUID(msg.data(), zeros, 0))	{
                            stage = constants::ccp::ccp_stages;
                        } else {
                            if ((stage == (msg.data()[7] & 0xf)) && compareUID(msg.data(), uid_str, stage) ) {
                                // gehe in den naechsten Zyklus
                                --stage;
                            } else {
                                ccp_status = CCP_WAIT;
                            }
                        }

                    } else {
                        // die BrokerNachricht enthaelt keinen Stagezaehler mehr, dafuer aber eine
                        // konfigurierte KnotenID, weil sich mindestens ein Knoten in Stage 0 befindet
                        // dies koennte ich sein. Einfach mal die uid mit den Daten vergleichen

                        // untersuche Nachrichteninhalt auf Gleichheit mit eigener ID
                        // wenn ja, extrahierte txnode ist meine
                        // verlasse beide Schleifen da stage==0 und ccp_status==CCP_RUNNING
                        if (compareUID(msg.data(), uid_str, 0)) {
//                            debug("Knoten ID ist meine\n");
                            ccp_stage = CCP_COMPLETE;
                        } else {
                            // wenn nein einfach von vorn
                            // und gehe in die aeussere Schleife weil ccp_status==CCP_RUNNING
                            // und setze stage wieder auf den initialen Wert
//                            debug("Knoten ID ist nicht meine\n");
                            stage = constants::ccp::ccp_stages;
                        }
                    }
                } while ( (ccp_status != CCP_RUNNING) );
            } while ( (ccp_stage != CCP_COMPLETE) );
            // Konfiguration erfolgreich und KnotenID erhalten
            return tx_node;
        }

        bool handle_ccp_configure_request(typename CAN_Driver::MOB&, CAN_Driver&) {
            return false;
        };
};
}
} /* namespace CAN */
} /* namespace nl */
} /* namespace mw */
} /* namespace famouso */


#endif
