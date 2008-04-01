#include "devices/nic/ip/socket/UDPSocket.h"  // For UDPSocket and SocketException
#include <iostream>           // For cout and cerr
#include <cstdlib>            // For atoi()

#ifdef WIN32
#include <windows.h>          // For ::Sleep()
void sleep(unsigned int seconds) {::Sleep(seconds * 1000);}
#else
#include <unistd.h>           // For sleep()
#endif

using namespace std;

int main(int argc, char *argv[]) {
  if ((argc < 3) || (argc > 4)) {   // Test for correct number of arguments
    cerr << "Usage: " << argv[0] 
         << " <Destination Address> <Destination Port> <Send String>\n";
    exit(1);
  }

  string destAddress = argv[1];             // First arg:  destination address
  unsigned short destPort = atoi(argv[2]);  // Second arg: destination port
  char* sendString = argv[3];               // Third arg:  string to broadcast

  try {
    UDPSocket sock;
  
    // Repeatedly send the string (not including \0) to the server
    for (;;) {
      sock.sendTo(sendString, strlen(sendString), destAddress, destPort);
      sleep(3);
    }
  } catch (SocketException &e) {
    cerr << e.what() << endl;
    exit(1);
  }
  
  return 0;
}
