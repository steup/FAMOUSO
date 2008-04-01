#ifndef __GenericSocket_h__
#define __GenericSocket_h__


#ifndef WIN32
	#include "devices/nic/ip/socket/bsd/BSDSocket.h"
	typedef BSDSocket	GenericSocket;
#else
	#include "devices/nic/ip/socket/win32/WinSocket.h"
	typedef WinSocket	GenericSocket;
#endif

#endif
