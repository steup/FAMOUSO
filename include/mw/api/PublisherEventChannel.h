#ifndef __PublisherEventChannel_h__
#define __PublisherEventChannel_h__

#include "mw/api/EventChannel.h"

namespace famouso {
  namespace mw {
    namespace api {
      
      template < class ECH >
      class PublisherEventChannel : public EventChannel<ECH> {
	
      public:
	
        PublisherEventChannel(const Subject &s) : EventChannel<ECH>(s) {
	}
	
	~PublisherEventChannel() {
	  unannounce();
	}
	
	
	/*! \brief announce the event channel and reserve resources
	 *         and call the needed functionalities within the
	 *         sublayers to bind the subject etc.
	 *
	 *         \sa EventLayer::announce
	 *         \sa AbstractNetworkLayer::announce
	 */
	void announce() {
	  DEBUG(("%s\n", __PRETTY_FUNCTION__));
	  EventChannel<ECH>::ech().announce(*this);
	}
	
	
	/*! \brief publish an event via the event channel
	 *
	 *         \sa EventLayer::publish
	 *         \sa AbstractNetworkLayer::publish
	 */
	void publish(const Event& e) {
	  DEBUG(("%s\n", __PRETTY_FUNCTION__));
	  EventChannel<ECH>::ech().publish(*this,e);
	}
	
      private:
	void unannounce() {
	  DEBUG(("%s\n", __PRETTY_FUNCTION__));
	  EventChannel<ECH>::ech().unannounce(*this);
	}
	
      };
      
    } // namespace api
  } // namespace mw
} // namespace famouso

#endif /* __PublisherEventChannel_h__ */
