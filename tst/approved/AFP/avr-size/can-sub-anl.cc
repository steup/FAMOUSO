// AVR size test: subscriber; no AFP in Application Layer, AFP in ANL; CAN NL; Max. event 255B
#define EVENT_LENGTH 255

#define AFP_ANL_CONFIG_DEF
#define AFP_ANL_CONFIG_FRAG famouso::mw::afp::Disable
#define AFP_ANL_CONFIG_DEFRAG nl::AFP_Config

#include "can.h"

typedef famouso::config::SEC SEC_Type;

#include "subscribe.impl"

