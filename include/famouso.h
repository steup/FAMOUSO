#ifndef __famouso_h__
#define __famouso_h__

struct FAMOUSO {
  enum {
    SUBSCRIBE=0x50,
    UNSUBSCRIBE,
    PUBLISH,

    ANNOUNCE=0x56
  };
};

#define echoServPort 5005

#endif
