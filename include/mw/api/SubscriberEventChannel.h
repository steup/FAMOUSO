#ifndef __SubscriberEventChannel_h__
#define __SubscriberEventChannel_h__

#include "mw/api/EventChannel.h"

template < class EC >
class SubscriberEventChannel : public EC {
  void announce();
  void publish(const Event& e);
 public:
  
 SubscriberEventChannel(const Subject &s) : EC(s) {}
 
  ~SubscriberEventChannel() {
/*     EventChannel<ECH>::unsubscribe();  */
    EC::unsubscribe(); 
  }
  
};


#endif /* __SubscriberEventChannel_h__ */
