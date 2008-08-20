#ifndef __WinSocket_h__
#define __WinSocket_h__

#include "devices/nic/ip/socket/SocketDescriptor.h"
#include "devices/nic/ip/socket/win32/WinSocketBase.h"

#include <winsock2.h>		// For socket(), connect(), send(), and recv()
#include <iostream>

typedef int socklen_t;
typedef char raw_type;		// Type used for raw data on this platform

/**
 *   Base class representing basic communication endpoint
 */
class WinSocket : public SocketDescriptor {
    static WinSocketBase wsb;
public:
    /**
    *   Close and deallocate this socket
    */
    ~WinSocket();
    void analyse() {
        switch (WSAGetLastError()) {
        case WSANOTINITIALISED: {
            cout<<  "successful WSAStartup() must occur before using this API" <<endl;
            break;
        }
        case WSAENETDOWN: {
            cout<< "The Windows Sockets implementation has detected that the network subsystem has failed"<<endl;
            break;
        }
        case WSAEAFNOSUPPORT: {
            cout<< "The specified address family is not supported"<<endl;
            break;
        }
        case	WSAEINPROGRESS: {
            cout<< "A blocking Windows Sockets operation is in progress"<<endl;
            break;
        }
        case	WSAEMFILE: {
            cout<< "No more file descriptors are available"<<endl;
            break;
        }

        case	WSAENOBUFS: {
            cout<< "No buffer space is available. The socket cannot be created"<<endl;
            break;
        }
        case	WSAEPROTONOSUPPORT: {
            cout<< "The specified protocol is not supported"<<endl;
            break;
        }
        case	WSAEPROTOTYPE: {
            cout<< "The specified protocol is the wrong type for this socket"<<endl;
            break;
        }
        case	WSAESOCKTNOSUPPORT: {
            cout<< "The specified socket type is not supported in this address family"<<endl;
            break;
        }
        }

    }
};

#endif
