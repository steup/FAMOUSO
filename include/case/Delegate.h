#ifndef __Delagate_h__
#define __Delagate_h__

namespace famouso {
	namespace util {

/*!
 *		\class Delegate
 *		\brief Implements anonymous callback functionality
 */
template<typename parameter>
class Delegate {
  typedef void (*invoke_stub)(void const *, parameter);
  void const  *obj_ptr_;
  invoke_stub stub_ptr_;

  template<typename T, void (T::*Fxn)(parameter)>
    struct mem_fn_stub
    {
      static void invoke(void const * obj_ptr, parameter a0)
      {
	T * obj = static_cast<T *>( const_cast<void *>( obj_ptr ) );
	(obj->*Fxn)( a0 );
      }
    };

  template<typename T, void (T::*Fxn)(parameter) const>
    struct mem_fn_const_stub
    {
      static void invoke(void const * obj_ptr, parameter a0)
      {
	T const * obj = static_cast<T const *>( obj_ptr );
	(obj->*Fxn)( a0 );
      }
    };

  template<void (*Fxn)(parameter)>
    struct function_stub
    {
      static void invoke(void const *, parameter a0)
      {
	(*Fxn)( a0 );
      }
    };

 public:
 Delegate() : obj_ptr_( 0 ), stub_ptr_( 0 ) { }

  template<typename T, void (T::*Fxn)(parameter)>
    void from_function(T * obj)
    {
      obj_ptr_ = const_cast<T const *>( obj );
      stub_ptr_ = &mem_fn_stub<T, Fxn>::invoke;
    }

  template<typename T, void (T::*Fxn)(parameter) const>
    void from_function(T const * obj)
    {
      obj_ptr_ = obj;
      stub_ptr_ = &mem_fn_const_stub<T, Fxn>::invoke;
    }

  template<void (*Fxn)(parameter)>
    void from_function()
    {
      obj_ptr_ = 0;
      stub_ptr_ = &function_stub<Fxn>::invoke;
    }

  void operator ()(parameter a0) const
  {
    ( *stub_ptr_ )( obj_ptr_, a0 );
  }
};
	} // namespace util
} // namespace famouso
#endif
