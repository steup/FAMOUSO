#ifndef __SubscriberEventChannel_h__
#define __SubscriberEventChannel_h__

#include "mw/api/EventChannel.h"

namespace famouso {
    namespace mw {
        namespace api {

            template < class ECH >
            class SubscriberEventChannel : public EventChannel<ECH> {

                public:
                    /*! \brief This callback is called if an event occurs.
                     *
                     *  CallBack is a generic delegate whish allows for using
                     *  C-functions, normal as well as const class member
                     *  functions.
                     */
                    famouso::mw::api::SECCallBack callback;

                public:

                    SubscriberEventChannel(const Subject &s) : EventChannel<ECH>(s) {}

                    ~SubscriberEventChannel() {
                        unsubscribe();
                    }

                    /*! \brief subscribe the event channel and reserve resources
                     *         and call the needed functionalities within the
                     *         sublayers to bind the subject etc.
                     *
                     *         \sa EventLayer::subscribe
                     *         \sa AbstractNetworkLayer::subscribe
                     */
                    void subscribe() {
                        DEBUG(("%s %p\n", __PRETTY_FUNCTION__, this));
                        // initialize a dummy callback
                        callback.bind<&famouso::mw::api::cb>();

                        EventChannel<ECH>::ech().subscribe(*this);
                    }

                private:
                    void unsubscribe() {
                        DEBUG(("%s\n", __PRETTY_FUNCTION__));
                        EventChannel<ECH>::ech().unsubscribe(*this);
                    }

            };

        } // namespace api
    } // namespace mw
} // namespace famouso


#endif /* __SubscriberEventChannel_h__ */
