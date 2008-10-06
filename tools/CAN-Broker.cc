#include "devices/nic/can/peak/PeakCAN.h"
#include "mw/common/UID.h"

char *dev = "/dev/pcan32";

typedef device::nic::CAN::PeakCAN<dev, 0x011c> can;
typedef can::MOB	mob;
can canDriver;

void callback() {
    std::cout << "rx_Interrupt from CAN -- Callback " << std::endl;
}

namespace famouso {
    namespace mw {
        namespace nl {
            namespace CAN {
                namespace ETAGS {
                    enum {
                        CCP_SSI		= 0x0,
                        CCP_RSI,
                        GET_ETAG,
                        SUPPLY_ETAG
                    };
                }
                union ID {
                    uint32_t value;
                    class __attribute__((packed)) {
                        uint16_t _etag      :
                            14;
                        uint8_t  _tx_nodelo :
                            2;
                        uint8_t  _tx_nodehi :
                            5;
                        int8_t   _priolo    :
                            3;
                        int8_t   _priohi    :
                            5;
                        uint8_t             :
                            3;
                        public:
                            uint16_t etag() {
                                return _etag;
                            }
                            void etag(uint16_t e) {
                                _etag = e;
                            }
                            uint8_t tx_node() {
                                return _tx_nodelo | (_tx_nodehi << 2) ;
                            }
                            void tx_node(uint8_t t) {
                                _tx_nodelo = t & 0x3;
                                _tx_nodehi = t >> 2 ;
                            }
                            uint8_t prio() {
                                return _priolo | (_priohi << 3) ;
                            }
                            void prio(uint8_t p) {
                                _priolo = p & 0x7;
                                _priohi = p >> 3;
                            }
                    } parts;

                    class __attribute__((packed)) {
                        uint16_t _etag    :
                            14;
                        uint8_t  _nibblelo :
                            2;
                        uint8_t  _nibblehi :
                            2;
                        uint8_t  _stage  :
                            4;
//                        uint16_t        : 9;
                        public:
                            uint16_t etag() {
                                return _etag;
                            }
                            uint8_t stage() {
                                return _stage;
                            }
                            uint8_t nibble() {
                                return _nibblelo | (_nibblehi << 2) ;
                            }
                    } parts_ccp;
                    uint8_t v[4];
                }__attribute__((packed));

            }
        }
    }
};

//
// damit der Algorithmus ordentlich funktioniert, ist eine
// Randbedingung, dass die UID = 0x0 von keinem Knoten
// verwendet wird.
//
template < class CAN_MOB>
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

            famouso::mw::nl::CAN::ID *id = reinterpret_cast<famouso::mw::nl::CAN::ID*>(&mob.ID);

            // extrahiere zunaechst die wichtigsten Infos
            uint8_t nibble = id->parts_ccp.nibble();
            uint8_t stage = id->parts_ccp.stage();
            uint8_t tx_node = constants::Broker_tx_node;

            std::cout	<< std::hex << "etag="	<< id->parts_ccp.etag()
            << "\tnibble=" << (uint32_t)id->parts_ccp.nibble()
            << "\tstage=" << (uint32_t)id->parts_ccp.stage() << std::endl;

            // kommt die Nachricht von jemandem der im richtigen stage ist?
            if (!(ccp_stage == stage)) {
                std::cerr << "received ccp rsi for stage " << (uint32_t)stage << " but I am in stage "
                << (uint32_t)ccp_stage << " dropping it." << std::endl;
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
                id->parts.prio(0xFD);

                id->parts.etag(famouso::mw::nl::CAN::ETAGS::CCP_SSI);

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
                        std::cout << std::hex << "UID 0x" << uid.value
                        << " [" << (char*)&uid.value << "] "
                        << "got short node id tx_node="
                        << (uint32_t)tx_node << std::endl;
                    }
                    ccp_stage = 15;
                    uid = 0;
                }
                // tx_node ist entweder die des Brokers, wenn noch nicht stage 0 und
                // sonst die txnode des neuen Knoten
                id->parts.tx_node(tx_node);
            }
            return true;
        }
};

CCP_Broker<mob> broker;

int main(int argc, char **argv) {
    //dev=argv[1];
    mob m;
    canDriver.init();
    //canDriver.set_rx_Interrupt(&callback);
    std::cout << "CAN Broker providing CAN Configuration Protocol and Binding Protocol" << std::endl;
    while (1) {
        canDriver.receive_blocking(&m);
        famouso::mw::nl::CAN::ID *id = reinterpret_cast<famouso::mw::nl::CAN::ID*>(&m.ID);
        switch ( id->parts.etag()) {
            case famouso::mw::nl::CAN::ETAGS::CCP_RSI: {
//            std::cout << "CAN Configuration Protocol request for nodeID" << std::hex << std::endl;
//            std::cout << "ID=" << id->value
//            << "\tetag=" << id->parts.etag()
//            << "\ttx_node=" << (uint32_t)id->parts.tx_node()
//            << "\tprio=" << (uint32_t)id->parts.prio() << std::endl;
                    if ( broker.handle_ccp_rsi(m) )
                        canDriver.send(m);
                    break;
                }
            case famouso::mw::nl::CAN::ETAGS::GET_ETAG: {
                    std::cout << "got ETAG_Request" << std::endl;
                }

        }
    }
    return 0;
}


