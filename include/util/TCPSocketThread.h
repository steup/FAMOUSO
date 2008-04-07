#ifndef __TCPSocketThread_h__
#define __TCPSocketThread_h__

#include "util/Thread.h"
#include "devices/nic/ip/socket/TCPSocket.h"


// Synchronisation der unterschiedlichen Threads beim Zugriff
// auf den ECH sollte geregelt werden
class TCPSocketThread : public Thread {
protected:
    TCPSocket * sock;

    TCPSocketThread (TCPSocket * s):sock (s) {
    }

    ~TCPSocketThread () {
        delete sock;
    }
};

#endif
