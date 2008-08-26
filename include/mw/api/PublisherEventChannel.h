#ifndef __PublisherEventChannel_h__
#define __PublisherEventChannel_h__

#include "mw/api/EventChannel.h"

namespace famouso {
	namespace mw {
		namespace api {
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

		} // namespace api
	} // namespace mw
} // namespace famouso

#endif /* __PublisherEventChannel_h__ */
