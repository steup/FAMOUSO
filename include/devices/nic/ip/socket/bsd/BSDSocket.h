#ifndef __BSDSocket_h__
#define __BSDSocket_h__

#include "devices/nic/ip/socket/SocketDescriptor.h"

#include <sys/types.h>       // For data types
#include <sys/socket.h>      // For socket(), connect(), send(), and recv()
#include <netdb.h>           // For gethostbyname()
#include <arpa/inet.h>       // For inet_addr()
#include <unistd.h>          // For close()
#include <netinet/in.h>      // For sockaddr_in

typedef void raw_type;       // Type used for raw data on this platform


/**
 *   BSDSocket class representing basic communication
 *   endpoint on BSD like systems
 */
class BSDSocket : public SocketDescriptor {
public:
	~BSDSocket();
	void analyse(){}
};

#endif
