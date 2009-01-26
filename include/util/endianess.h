#ifndef __endianness_h__
#define __endianness_h__

#ifndef WIN32
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

/**
 *   byte ordering function for 64bit variable
 *
 */
inline uint64_t htonll(uint64_t host_longlong)
{
    int x = 1;

	/* little endian */
    if(*(char *)&x == 1)
        return ((((uint64_t)htonl(host_longlong)) << 32) + htonl(host_longlong >> 32));

    /* big endian */
    else
        return host_longlong;
}

#define ntohll(x) htonll(x)

#endif
