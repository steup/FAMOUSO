#ifndef __Event_h__
#define __Event_h__

#include <stdint.h>
#include "mw/common/Subject.h"

struct Event {
  typedef uint16_t Type;
  Type length;
  uint8_t *data;
  const Subject &subject;


  Event (const Subject &s): length(0), data(0), subject (s) {}

  uint8_t & operator[](uint8_t i) {
    return data[i];
  }

  uint8_t & operator[](uint8_t i) const {
    return data[i];
  }
};

#endif /* __Event_h__ */
