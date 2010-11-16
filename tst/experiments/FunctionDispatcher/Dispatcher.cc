/*******************************************************************************
 *
 * Copyright (c) 2008-2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/

#include "debug.h"
#include "object/Chain.h"
#include "object/Queue.h"

/*
 * Implements anonymous callback functionality and it has the ability to be
 * queued. It uses the technique of CRTP in order to bind the callback with
 * the emit method of the derived class.
 *
 */
template < typename Derived=void>
class BaseInvoker : public Chain {
        typedef void (*invoke_stub)(void const *);
        volatile invoke_stub stub_ptr_;

        template < typename T >
        struct mem_fn_stub {
            static void invoke(void const * obj_ptr) {
                T * obj = static_cast<T *>(const_cast<void *>(obj_ptr));
                obj->emit();
            }
        };

    protected:
        BaseInvoker() : stub_ptr_ ( &mem_fn_stub<Derived>::invoke) {}

    public:
        void operator()() const {
            (*stub_ptr_)(this);
        }
};


/*
 * Define different callbacks to allow parameter binding, which are
 * forwarded to the bound Functor.
 */
template<typename Functor>
struct Callback0 : BaseInvoker<Callback0<Functor> > {
    void emit(){
        Functor()();
    }
};

template<typename Functor, typename T0>
struct Callback1 : BaseInvoker<Callback1<Functor, T0> > {
    T0 _t0;
    Callback1(T0 t0) : _t0(t0) {}
    void emit(){
        Functor()(_t0);
    }
};

template<typename Functor, typename T0, typename T1>
struct Callback2 : BaseInvoker<Callback2<Functor, T0, T1> > {
    T0 _t0;
    T1 _t1;
    Callback2(T0 t0, T1 t1) : _t0(t0), _t1(t1) {}
    void emit(){
        Functor()(_t0, _t1);
    }
};

struct Functor {
    void operator()(){
        ::logging::log::emit() << "F0" << ::logging::log::endl;
    }
    template<typename T0>
    void operator()(T0 &t0){
        ::logging::log::emit() << "F1 " << t0 << ::logging::log::endl;
    }
    template<typename T0, typename T1>
    void operator()(T0 &t0, T1 &t1){
        ::logging::log::emit() << "F2 " << t0 << " " << t1 << ::logging::log::endl;
    }
};

Callback0<Functor> cb0;
Callback1<Functor, int> cb1(10);
Callback2<Functor, int, void*> cb2(10, &cb1);

int main(int argc, char **argv) {
    Queue q;
    q.append(cb0);
    q.append(cb1);
    q.append(cb2);
    Chain *item;
    while ((item=q.unlink()))
       (*((BaseInvoker<>*)item))();
}
