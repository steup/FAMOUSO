
#include "devices/nic/can/peak/PeakCAN.h"
#include "mw/nl/can/canETAGS.h"
#include "mw/nl/can/canID.h"
#include "mw/nl/can/ETAG_Broker.h"
#include "mw/nl/can/CCP_Broker.h"
#include "mw/common/UID.h"

char *dev = "/dev/pcan32";

typedef device::nic::CAN::PeakCAN<dev, 0x011c> can;
typedef famouso::mw::nl::CAN::detail::ID ID;
typedef can::MOB	mob;

int main(int argc, char **argv) {
    std::cout << "Project: FAMOUSO" << std::endl;
    std::cout << "CAN Broker providing CAN Configuration Protocol and Binding Protocol" << std::endl << std::endl;
    std::cout << "Author: Michael Schulze" << std::endl;
    std::cout << "Revision: $Rev$" << std::endl;
    std::cout << "$Date$" << std::endl;
    std::cout << "last changed by $Author$" << std::endl << std::endl;
    std::cout << "build Time: " << __TIME__ << std::endl;
	std::cout << "build Date: " << __DATE__ << std::endl << std::endl;

	// allow /dev/pcanXX as parameter for PeakCAN driver
	if (argc > 1)
		dev=argv[1];

	can canDriver;
	famouso::mw::nl::CAN::CCP_Broker<mob> ccp_broker;
	famouso::mw::nl::CAN::ETAG_Broker<mob> etag_broker;

    canDriver.init();
    while (1) {
		mob m;
        canDriver.receive_blocking(&m);
        ID *id = reinterpret_cast<ID*>(&m.ID);
        switch ( id->etag()) {
            case famouso::mw::nl::CAN::ETAGS::CCP_RSI: {
                    if ( ccp_broker.handle_ccp_rsi(m) )
                        canDriver.send(m);
                    break;
                }
            case famouso::mw::nl::CAN::ETAGS::GET_ETAG: {
                    if ( etag_broker.get_etag(m) )
                        canDriver.send(m);
                    break;
                }

        }
    }
    return 0;
}


