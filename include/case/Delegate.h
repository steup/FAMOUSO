#ifndef __Delagate_h__
#define __Delagate_h__

namespace famouso {
    namespace util {

        /*!
         *  \class Delegate
         *  \brief Implements anonymous callback functionality
         */
        template < typename parameter = void >
        class Delegate {
                typedef void (*invoke_stub)(void const *, parameter);
                void const  *obj_ptr_;
                invoke_stub stub_ptr_;

                template < typename T, void (T::*Fxn)(parameter) >
                struct mem_fn_stub {
                    static void invoke(void const * obj_ptr, parameter a0) {
                        T * obj = static_cast<T *>(const_cast<void *>(obj_ptr));
                        (obj->*Fxn)(a0);
                    }
                };

                template < typename T, void (T::*Fxn)(parameter) const >
                struct mem_fn_const_stub {
                    static void invoke(void const * obj_ptr, parameter a0) {
                        T const * obj = static_cast<T const *>(obj_ptr);
                        (obj->*Fxn)(a0);
                    }
                };

                template < void (*Fxn)(parameter) >
                struct function_stub {
                    static void invoke(void const *, parameter a0) {
                        (*Fxn)(a0);
                    }
                };

            public:
                Delegate() : obj_ptr_(0), stub_ptr_(0) { }

                template < typename T, void (T::*Fxn)(parameter) >
                void bind(T * obj) {
                    obj_ptr_ = const_cast<T const *>(obj);
                    stub_ptr_ = &mem_fn_stub<T, Fxn>::invoke;
                }

                template < typename T, void (T::*Fxn)(parameter) const >
                void bind(T const * obj) {
                    obj_ptr_ = obj;
                    stub_ptr_ = &mem_fn_const_stub<T, Fxn>::invoke;
                }

                template < void (*Fxn)(parameter) >
                void bind() {
                    obj_ptr_ = 0;
                    stub_ptr_ = &function_stub<Fxn>::invoke;
                }

                //  additional attributes only support on newer compilers
                //  void operator ()(parameter a0) const __attribute__((always_inline))
                void operator()(parameter a0) const {
                    (*stub_ptr_)(obj_ptr_, a0);
                }

                operator bool() const {
                    return stub_ptr_;
                }
        };

        template<>
        class Delegate<void> {
                typedef void (*invoke_stub)(void const *);
                void const  *obj_ptr_;
                invoke_stub stub_ptr_;

                template < typename T, void (T::*Fxn)() >
                struct mem_fn_stub {
                    static void invoke(void const * obj_ptr) {
                        T * obj = static_cast<T *>(const_cast<void *>(obj_ptr));
                        (obj->*Fxn)();
                    }
                };

                template < typename T, void (T::*Fxn)() const >
                struct mem_fn_const_stub {
                    static void invoke(void const * obj_ptr) {
                        T const * obj = static_cast<T const *>(obj_ptr);
                        (obj->*Fxn)();
                    }
                };

                template < void (*Fxn)() >
                struct function_stub {
                    static void invoke(void const *) {
                        (*Fxn)();
                    }
                };

            public:
                Delegate() : obj_ptr_(0), stub_ptr_(0) { }

                template < typename T, void (T::*Fxn)() >
                void bind(T * obj) {
                    obj_ptr_ = const_cast<T const *>(obj);
                    stub_ptr_ = &mem_fn_stub<T, Fxn>::invoke;
                }

                template < typename T, void (T::*Fxn)() const >
                void bind(T const * obj) {
                    obj_ptr_ = obj;
                    stub_ptr_ = &mem_fn_const_stub<T, Fxn>::invoke;
                }

                template < void (*Fxn)() >
                void bind() {
                    obj_ptr_ = 0;
                    stub_ptr_ = &function_stub<Fxn>::invoke;
                }

                //  additional attributes only support on newer compilers
                //  void operator ()() const __attribute__((always_inline))
                void operator()() const {
                    (*stub_ptr_)(obj_ptr_);
                }

                operator bool() const {
                    return stub_ptr_;
                }
        };

    } // namespace util
} // namespace famouso
#endif
