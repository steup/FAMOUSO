/*******************************************************************************
 *
 * Copyright (c) 2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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


#include <iostream>
#include <stdio.h>

template < typename RetT = void >
class Delegate {
        // Static functions //
        template < typename ClassT, RetT(ClassT::*Method)() >
        static RetT methodPointerInvoker(void* data) {
            return (static_cast<ClassT*>(data)->*Method)();
        }

        // inner class //
        template<typename ClassT>
        class Proxy {
                Delegate* m_delegate;
            public:
                Proxy(Delegate* dg) :
                        m_delegate(dg) { }

                template < RetT(ClassT::*FUNC)() >
                void set() const {
                    m_delegate->m_invoker = &methodPointerInvoker<ClassT, FUNC>;
                }

                template < RetT(ClassT::*FUNC)() >
                void operator () () const {
                    m_delegate->m_invoker = &methodPointerInvoker<ClassT, FUNC>;
                }

        };

        // Attributes //
    private:
        RetT(*m_invoker)(void*);
        void* m_data;


        // Operators //
    public:
        RetT operator()() {
            return (*m_invoker)(m_data);
        }


        // Methods //
        template<typename ClassT>
        Proxy<ClassT> const bind(ClassT* obj) {
            m_data = obj;
            return Proxy<ClassT>(this);
        }

        template < class ClassT, RetT(ClassT::*Method)(void) >
        void bind(ClassT* obj) {
            m_data = obj;
            m_invoker = methodPointerInvoker <ClassT, Method>;
        }

};

struct A {
    inline void foo() {
        std::cout << "foo " << sizeof(Delegate<void>) << std::endl;
    }
};

int main() {
    A a, a1;

    Delegate<void> dg;
    Delegate<void> dg1;

    dg.bind(&a).set<&A::foo>();

    dg1.bind<A, &A::foo>(&a1);

    dg();
    dg1();

    return 0;
}
