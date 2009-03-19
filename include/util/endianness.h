#ifndef __endianness_h__
#define __endianness_h__

#include "util/swap_endianness.h"

#if defined(LINUX)
    #include <arpa/inet.h>

#elif defined(WIN32)
    #include <winsock2.h>

#elif defined(AVR)
    #define __BYTE_ORDER __LITTLE_ENDIAN
    #define ntohl(x) __swap_endianness_32(x)
    #define ntohs(x) __swap_endianness_16(x)
    #define htonl(x) __swap_endianness_32(x)
    #define htons(x) __swap_endianness_16(x)

#else
#error "It seems to be compiling for an unsupported platform\n and it needs to be defined the endianness of that system."

#endif
/**
 *   byte ordering function for 64bit variable
 *
 */
//inline uint64_t htonll(uint64_t host_longlong)
//{
//    int x = 1;
//
//	/* little endian */
//    if(*(char *)&x == 1)
//        return ((((uint64_t)htonl(host_longlong)) << 32) + htonl(host_longlong >> 32));
//
//    /* big endian */
//    else
//        return host_longlong;
//}
//

#if __BYTE_ORDER == __BIG_ENDIAN
///* The host byte order is the same as network byte order,
//   so these functions are all just identity.  */
# define htonll(x)  (x)
# define ntohll(x)  (x)
# else
#  if __BYTE_ORDER == __LITTLE_ENDIAN
#   define htonll(x)    __swap_endianness_64(x)
#   define ntohll(x)    __swap_endianness_64(x)
#  else
#   error "BYTE_ORDER is not defined"
#  endif
# endif

#endif
