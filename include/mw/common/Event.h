#ifndef __Event_h__
#define __Event_h__

#include <stdint.h>
#include "mw/common/Subject.h"

/* namespace cosmic { */
/* namespace EventLayer { */

class Event {
 public:
  typedef uint16_t Type;
  Type len;
  uint8_t *_data;
  const Subject &_subj;
 public:

  Event (const Subject &s): len(0), _subj (s) {}
  //  Event (uint8_t s) : len(0), _subj((uint8_t)s) {}

  void setLen(const Type &l) {
    len=l;
  }

  const Type& getLen() const {
    return len;
  }

  uint8_t & operator[](uint8_t i) {
    return _data[i];
  }

  uint8_t & operator[](uint8_t i) const {
    return _data[i];
  }
};

/* } */
/* } */

#endif /* __Event_h__ */
