#include "mw/common/CallBack.h"

#include "debug.h"

namespace famouso {
	namespace mw {
		namespace api {
 /*! \brief cb is the default callback
   *
   * \note Probably not needed, because of defining callback by the application
   * itself. However for testing is nice to have. Future versions will remove
   * that function.
   */
  void cb(SECCallBackData& cbd) {
    DEBUG(("%s Parameter=%d Daten:=%s\n", __PRETTY_FUNCTION__, cbd.length, cbd.data));
  }
		}
	}
}
