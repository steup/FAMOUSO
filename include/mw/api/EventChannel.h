#ifndef __EventChannel_h__
#define __EventChannel_h__

#include "mw/common/Subject.h"
#include "mw/common/Event.h"
#include "mw/common/AttributeList.h"
#include "mw/common/CallBack.h"

#include "debug.h"
#include "object/Chain.h"
#include "case/Delegate.h"

namespace famouso {
	namespace mw {
		namespace api {

/*! \file EventChannel.h
 *
 * \class EventChannel
 *
 * \brief This is the base class of each EventChannel
 *
 *  The EventChannel encapsulates the needed data structures to handle the
 *  publish/subscribe communication and implements the redirection of the calls
 *  to the EventLayer also.
 *
 * \todo open question, whether the ECH should be static or accessible via a
 * singleton ???
 */
template < class ECH >
class EventChannel : public Chain {
  // definition of the event channel handler
  static uint8_t _ech[sizeof(ECH)];
  // the 64Bit subject
  Subject _subj;
  // definition of the short network names of a subjectes
  typename ECH::SNN _snn;

 public:
  /*! \brief get the local event channel handler object
   */
  static ECH& ech() {
    return reinterpret_cast<ECH&>(_ech);
  }

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


 protected:
  /*! \brief Constructor of the class is protected to avoid
   *         instanciating objects.
   */
  EventChannel(const Subject& s) : _subj(s) {
    DEBUG(("%s ECH-ADDR=%p\n", __PRETTY_FUNCTION__,&_ech));
  }

};

/*! \brief The local event channel handler object.
 *
 * \todo open question, whether the ECH should be static or accessible via a singleton ???
 */
template < class ECH > uint8_t EventChannel<ECH>::_ech[sizeof(ECH)]={0};

		} // namespace api
	} // namespace mw
} // namespace famouso


#endif /* __EventChannel_h__ */
