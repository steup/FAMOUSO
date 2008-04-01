#include "devices/nic/ip/socket/win32/WinSocket.h"

WinSocket::~WinSocket() {
	closesocket(sockDesc);
}


WinSocketBase WinSocket::wsb;
