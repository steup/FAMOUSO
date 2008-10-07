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
	std::cout << "CAN Configuration Protocol Client Test Program" << std::endl << std::endl;
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
	famouso::mw::nl::CAN::ccp::Client<can> ccp_client;
	canDriver.init();

	std::cout << "KnotenID = 0x" << std::hex
		  << static_cast<uint32_t>(ccp_client.ccp_run("Michael\0",canDriver))
		  << std::endl;

	return 0;
}


