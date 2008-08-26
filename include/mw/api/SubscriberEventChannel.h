#ifndef __SubscriberEventChannel_h__
#define __SubscriberEventChannel_h__

#include "mw/api/EventChannel.h"

namespace famouso {
	namespace mw {
		namespace api {

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

		} // namespace api
	} // namespace mw
} // namespace famouso


#endif /* __SubscriberEventChannel_h__ */
