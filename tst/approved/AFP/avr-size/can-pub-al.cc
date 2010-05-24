// AVR size test: publisher; AFP in Application Layer, no AFP in ANL; CAN NL; Max. event 255B
#define EVENT_LENGTH 255

#define AFP_ANL_CONFIG_DEF
#define AFP_ANL_CONFIG_FRAG famouso::mw::afp::Disable
#define AFP_ANL_CONFIG_DEFRAG famouso::mw::afp::Disable

#include "can.h"
#include "mw/afp/AFPPublisherEventChannel.h"

typedef famouso::mw::afp::AFPPublisherEventChannel<famouso::config::PEC, MinAFPConfig, 8> PEC_Type;

#include "publish.impl"

