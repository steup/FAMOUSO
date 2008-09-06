#ifndef __CallBack_h__
#define __Callback_h__

#include "mw/common/Event.h"

#include "debug.h"
#include "case/Delegate.h"

namespace famouso {
	namespace mw {
		namespace aux {
 /*! \brief cb is the default callback
   *
   * \todo Probably not needed, because of defining callback by the application
   * itself. However for testing is nice to have. Future versions will remove
   * that function.
   */
  // Definition of CallBackData as well as the callback itself using a delegate
  // mechanism
  typedef const Event CallBackData; typedef
  famouso::util::Delegate<CallBackData&> CallBack;

  void cb(CallBackData& cbd);

		}
	}
}

#endif /* __CallBack_h__ */
