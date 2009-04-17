#ifndef __famouso_h__
#define __famouso_h__

// Initalisierungsfunktion, die mit dem richtigen Template
// aufgerufen werden muss, welches der ECH ist.
//
// using horrible hack to avoid using a static function for
// geting the ech instance
namespace famouso {
    template <class T>
    inline void init () {
        T *t=0;
        static_cast<T&>(*t).ech().init();
    }
}

#endif
