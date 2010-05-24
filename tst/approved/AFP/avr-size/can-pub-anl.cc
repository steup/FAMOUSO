// AVR size test: publisher; no AFP in Application Layer, AFP in ANL; CAN NL; Max. event 255B
#define EVENT_LENGTH 255

#define AFP_ANL_CONFIG_DEF
#define AFP_ANL_CONFIG_FRAG nl::AFP_Config
#define AFP_ANL_CONFIG_DEFRAG famouso::mw::afp::Disable

#include "can.h"

typedef famouso::config::PEC PEC_Type;

#include "publish.impl"

