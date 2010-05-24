// AVR size test: subscriber; no AFP at all; CAN NL; Max. event 8B
#define EVENT_LENGTH 8

#define AFP_ANL_CONFIG_DEF
#define AFP_ANL_CONFIG_FRAG famouso::mw::afp::Disable
#define AFP_ANL_CONFIG_DEFRAG famouso::mw::afp::Disable

#include "can.h"

typedef famouso::config::SEC SEC_Type;

#include "subscribe.impl"

