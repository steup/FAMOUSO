#ifndef __PublisherEventChannel_h__
#define __PublisherEventChannel_h__

#include "mw/api/EventChannel.h"

template < class EC >
class PublisherEventChannel : public EC {
  void subscribe();
 public:
  
 PublisherEventChannel(const Subject &s) : EC(s) {
  }
  
  ~PublisherEventChannel() {
    EC::unannounce(); 
  }
};

#endif /* __PublisherEventChannel_h__ */
