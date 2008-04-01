#ifndef __WinSocketBase_h__
#define __WinSocketBase_h__

#include "devices/nic/ip/socket/SocketException.h"
/**
 *   Base class representing basic communication endpoint
 */
class WinSocketBase {
public:
	WinSocketBase() throw(SocketException);

	/**
	*   Close and deallocate this socket
	*/
	~WinSocketBase() throw(SocketException);
};

#endif
