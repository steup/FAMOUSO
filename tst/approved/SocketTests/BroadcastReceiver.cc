#include "devices/nic/ip/socket/UDPSocket.h"   // For UDPSocket and SocketException
#include <iostream>            // For cout and cerr
#include <cstdlib>             // For atoi()

const int MAXRCVSTRING = 4096; // Longest string to receive

int main(int argc, char *argv[]) {

  if (argc != 2) {                  // Test for correct number of parameters
    cerr << "Usage: " << argv[0] << " <Local Port>" << endl;
    exit(1);
  }

  unsigned short echoServPort = atoi(argv[1]);     // First arg:  local port

  try {
    UDPSocket sock(echoServPort);                
  
    char recvString[MAXRCVSTRING + 1]; // Buffer for echo string + \0
    string sourceAddress;              // Address of datagram source
    unsigned short sourcePort;         // Port of datagram source
    int bytesRcvd = sock.recvFrom(recvString, MAXRCVSTRING, sourceAddress, 
                                  sourcePort);
    recvString[bytesRcvd] = '\0';  // Terminate string
    
    cout << "Received " << recvString << " from " << sourceAddress << ": "
         << sourcePort << endl;
  } catch (SocketException &e) {
    cerr << e.what() << endl;
    exit(1);
  }

  return 0;
}
