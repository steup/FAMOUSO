#include "devices/nic/can/peak/PeakCAN.h"

char *dev="/dev/pcan32";

typedef device::nic::CAN::PeakCAN<dev, 0x011c> can;

void callback(){
	std::cout << "rx_Interrupt from CAN -- Callback " << std::endl;
}

int main(int argc, char **argv){
	if (argc > 1 )
		dev=argv[1];

	// create driver and message object
	can p;
	can::MOB m;
	// initialize driver
	p.init();

	p.set_rx_Interrupt(&callback);

	std::cout << "Initalising completed." << std::endl;
	// test functionality depend on what is
	// in the message queue
	if (p.receive(&m)) {
		p.send(m);
	} else {
		std::cout << "nothing in the receive queue " << std::endl;
		p.receive_blocking(&m);
	}
	std::cout << "ID 0x" << std::hex << m.ID <<std::endl;
	std::cout << "Testprogramm done." << std::endl;
	return 0;
}
