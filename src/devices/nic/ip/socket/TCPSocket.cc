#include "devices/nic/ip/socket/TCPSocket.h"
// TCPSocket Code

TCPSocket::TCPSocket() 
    throw(SocketException) : CommunicatingSocket(SOCK_STREAM, 
    IPPROTO_TCP) {
}

TCPSocket::TCPSocket(const string &foreignAddress, unsigned short foreignPort)
    throw(SocketException) : CommunicatingSocket(SOCK_STREAM, IPPROTO_TCP) {
  connect(foreignAddress, foreignPort);
}

TCPSocket::TCPSocket(int newConnSD) : CommunicatingSocket(newConnSD) {
}

