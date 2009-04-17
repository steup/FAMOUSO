#ifndef __EventLayerCallBack_h__
#define __EventLayerCallback_h__

#include "mw/nl/BaseNL.h"
#include "case/Delegate.h"

namespace famouso {
    namespace mw {
        namespace el {

            typedef famouso::mw::nl::BaseNL* EL_CallBackData;
            typedef famouso::util::Delegate<EL_CallBackData> EL_CallBack;

            extern EL_CallBack IncommingEventFromNL;
        }
    }
}

#endif /* __CallBack_h__ */
