#include "devices/nic/ip/socket/win32/WinSocketBase.h"
#include "debug.h"
#include <winsock.h>		// For socket(), connect(), send(), and recv()

#include <iostream>

WinSocketBase::WinSocketBase() throw(SocketException) {
	DEBUG(("%s\n",__PRETTY_FUNCTION__));
      WORD wVersionRequested;
      WSADATA wsaData;

      wVersionRequested = MAKEWORD(2, 0);              // Request WinSock v2.0
      if (WSAStartup(wVersionRequested, &wsaData) != 0) {  // Load WinSock DLL
        throw SocketException("Unable to load WinSock DLL");
      }
}

WinSocketBase::~WinSocketBase() throw(SocketException) {
	DEBUG(("%s\n",__PRETTY_FUNCTION__));
    if (WSACleanup() != 0) {
      throw SocketException("WSACleanup() failed");
    }
}

