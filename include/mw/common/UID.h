#ifndef __UID_h__
#define __UID_h__

#include <stdint.h>

struct UID {
    union {
	uint64_t value;
	uint8_t tab[8];
    };
    
    UID() {} 
    UID(uint8_t uid)  : value(uid) {}
    UID(uint16_t uid) : value(uid) {}
    UID(uint32_t uid) : value(uid) {}
    UID(uint64_t uid) : value(uid) {}

    bool operator == (const UID &a) const{
	return value==a.value;
    }

//     const UID& 
};

#endif /* __UID_h__ */

/* This stuff is for emacs
 * Local variables:
 * mode:c++
 * c-basic-offset: 4
 * End:
 */
