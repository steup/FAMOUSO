// AVR size test: subscriber; no AFP in Application Layer, AFP in ANL; CAN NL; Max. event 255B
#define EVENT_LENGTH 255

#define AFP_ANL_CONFIG_DEF
#define AFP_ANL_CONFIG_FRAG famouso::mw::afp::Disable
#define AFP_ANL_CONFIG_DEFRAG nl::AFP_Config

#define DEFAULT_RAWSTORAGEALLOCATOR_MEM_SIZE (EVENT_LENGTH + 16)

#include "can.h"

typedef famouso::config::SEC SEC_Type;

#include "subscribe.impl"

