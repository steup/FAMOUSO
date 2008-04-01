#ifndef __EventChannel_h__
#define __EventChannel_h__

#include "mw/common/Subject.h"
#include "mw/common/Event.h"
#include "mw/common/AttributeList.h"

#include "debug.h"
#include "object/Chain.h"
#include "case/Delegate.h"

/*! \class EventChannel
 *  \brief This is the base class of each EventChannel
 *
 *  The EventChannel encapsulates the needed data structures to handle
 *  the publish/subscribe communication and implements the redirection
 *  of the calls to the EventLayer also.
 *
 * \todo open question, whether the ECH should be static or accessible via a singleton ???
 */
template < class ECH >
class EventChannel : public Chain {
  // definition of the event channel handler
  static ECH _ech;
  // the 64Bit subject
  Subject _subj;
  // definition of the short network names of a subjectes
  typename ECH::SNN _snn;


 public:
  /*! \brief give the Short Network Name representation of
   *         the subject corresponding to that event channel
   */
  typename ECH::SNN& snn() {
    return _snn;
  }

  /*! \brief give the Short Network Name representation of
   *         the subject corresponding to that event channel
   *         in case of a const object instance
   */
  const typename ECH::SNN& snn() const {
    return _snn;
  }

  /*! \brief give the 64Bit subject of that event channel
   */
  const Subject &subject() const {
    return _subj;
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
    _ech.announce(*this);
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
    _ech.subscribe(*this);
  }

  /*! \brief publish an event via the event channel
   *
   *         \sa EventLayer::publish
   *         \sa AbstractNetworkLayer::publish
   */
  void publish(const Event& e) {
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    _ech.publish(*this,e);
  }

  // Definition of CallBackData as well as the callback itself
  // using a delegate mechanism
  typedef const Event CallBackData;
  Delegate<CallBackData&> callback;


  /*! \brief cb is the default callback
   *
   *         \sa EventLayer::publish
   *         \sa AbstractNetworkLayer::publish
   *
   *   \todo Probably not needed, because of defining callback by
   *         the application itself. However for testing is nice
   *         to have. Future version will remove that function.
   */
  void cb(CallBackData& cbd) {
    DEBUG(("%s Parameter=%d Daten:=%s\n", __PRETTY_FUNCTION__, cbd.getLen(), cbd._data));
  }

 protected:
  /*! \brief Constructor of the class is protected to avoid
   *         instanciating objects.
   */
  EventChannel(const Subject& s) : _subj(s) {
    DEBUG(("%s ECH-ADDR=%p\n", __PRETTY_FUNCTION__,&_ech));
  }


  void unannounce() {
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    _ech.unannounce(*this);
  }

  void unsubscribe() {
    DEBUG(("%s\n", __PRETTY_FUNCTION__));
    _ech.unsubscribe(*this);
  }


};

/*! \brief The local event channel handler object.
 *
 * \todo open question, whether the ECH should be static or accessible via a singleton ???
 */
template < class ECH > ECH EventChannel<ECH>::_ech;

#endif /* __EventChannel_h__ */
