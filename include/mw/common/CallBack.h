#ifndef __CallBack_h__
#define __Callback_h__

#include "mw/common/Event.h"
#include "mw/nl/BaseNL.h"

#include "debug.h"
#include "case/Delegate.h"

namespace famouso {
	namespace mw {
		namespace api {

  /*! \brief Definition of the data that the SubscriberEventChannel callback will get as parameter
   */
  typedef const Event SECCallBackData;

  /*! \brief Definition of the callback itself using the CallBackData as template
   */
  typedef famouso::util::Delegate<SECCallBackData&> SECCallBack;


 /*! \brief cb is the default callback
   *
   * \note Probably not needed, because of defining callback by the application
   * itself. However for testing is nice to have. Future versions will remove
   * that function.
   */
  void cb(SECCallBackData& cbd);

		}
	}
}

#endif /* __CallBack_h__ */
