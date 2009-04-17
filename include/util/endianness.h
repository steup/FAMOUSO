/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/

#ifndef __endianness_h__
#define __endianness_h__

#include "util/swap_endianness.h"

#if defined(LINUX)
#include <arpa/inet.h>
#if __BYTE_ORDER == __BIG_ENDIAN
// The host byte order is the same as network byte order,
// so these functions are all just identity.
# define htonll(x)  (x)
# define ntohll(x)  (x)
#else
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define htonll(x)    __swap_endianness_64(x)
#define ntohll(x)    __swap_endianness_64(x)
#else
#error "BYTE_ORDER is not defined"
#endif
#endif


#elif defined(WIN32) || defined(AVR)
// Windows and AVR are both little endian
#if defined(WIN32)
#include <winsock2.h>
#endif

#if defined(AVR)
#define ntohl(x) __swap_endianness_32(x)
#define ntohs(x) __swap_endianness_16(x)
#define htonl(x) __swap_endianness_32(x)
#define htons(x) __swap_endianness_16(x)
#endif

#define htonll(x)    __swap_endianness_64(x)
#define ntohll(x)    __swap_endianness_64(x)

#else
#error "It seems to be compiling for an unsupported platform and it needs to be defined the endianness of that system."
#endif

#endif
