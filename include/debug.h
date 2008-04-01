#ifndef __DEBUG_h__
#define __DEBUG_h__

#ifdef NDEBUG

#define DEBUG(X)
#else
#include <stdio.h>
#define DEBUG(X) printf X
#endif

#endif
