#ifndef __CallBack_h__
#define __Callback_h__

#include "mw/common/CallBack.h"

#include "debug.h"

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

  void cb(CallBackData& cbd) {
    DEBUG(("%s Parameter=%d Daten:=%s\n", __PRETTY_FUNCTION__, cbd.length, cbd.data));
  }
		}
	}
}

#endif /* __CallBack_h__ */
