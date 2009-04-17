#ifndef __famouso_h__
#define __famouso_h__

namespace famouso {

    /*! \brief  The init function of FAMOUSO initialize the Event Channel
     *          Handler, its sub-objects and let all configuration protocols
     *          run.
     */
    template <class T>
    inline void init() {
        T::ech().init();
    }
}

#endif
