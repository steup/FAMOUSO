#ifndef __Socket_h__
#define __Socket_h__

#include "devices/nic/ip/socket/SocketException.h"
#include "devices/nic/ip/socket/GenericSocket.h"

/**
 *   Base class representing basic communication endpoint
 */
class Socket : public GenericSocket {
public:
  /**
   *   Close and deallocate this socket
   */
//  ~Socket();

  /**
   *   Get the local address
   *   @return local address of socket
   *   @exception SocketException thrown if fetch fails
   */
  string getLocalAddress() throw(SocketException);

  /**
   *   Get the local port
   *   @return local port of socket
   *   @exception SocketException thrown if fetch fails
   */
  unsigned short getLocalPort() throw(SocketException);

  /**
   *   Set the local port to the specified port and the local address
   *   to any interface
   *   @param localPort local port
   *   @exception SocketException thrown if setting local port fails
   */
  void setLocalPort(unsigned short localPort) throw(SocketException);

  /**
   *   Set the local port to the specified port and the local address
   *   to the specified address.  If you omit the port, a random port 
   *   will be selected.
   *   @param localAddress local address
   *   @param localPort local port
   *   @exception SocketException thrown if setting local port or address fails
   */
  void setLocalAddressAndPort(const string &localAddress, 
    unsigned short localPort = 0) throw(SocketException);

//  /**
//   *   If WinSock, unload the WinSock DLLs; otherwise do nothing. I ignore
//   *   this in the sample code but include it in the library for
//   *   completeness.  If you are running on Windows and you are concerned
//   *   about DLL resource consumption, call this after you are done with all
//   *   Socket instances.  If you execute this on Windows while some instance of
//   *   Socket exists, you are toast.  For portability of client code, this is
//   *   an empty function on non-Windows platforms so you can always include it.
//   *   @param buffer buffer to receive the data
//   *   @param bufferLen maximum number of bytes to read into buffer
//   *   @return number of bytes read, 0 for EOF, and -1 for error
//   *   @exception SocketException thrown WinSock clean up fails
//   */
//  static void cleanUp() throw(SocketException);

  /**
   *   Resolve the specified service for the specified protocol to the
   *   corresponding port number in host byte order
   *   @param service service to resolve (e.g., "http")
   *   @param protocol protocol of service to resolve.  Default is "tcp".
   */
  static unsigned short resolveService(const string &service,
                                       const string &protocol = "tcp");

private:
  // Prevent the user from trying to use value semantics on this object
  Socket(const Socket &sock);
  void operator=(const Socket &sock);

protected:
  Socket(int type, int protocol) throw(SocketException);
  Socket(int sockDesc);

  void fillAddr(const string&, unsigned short, sockaddr_in&);
};

#endif
