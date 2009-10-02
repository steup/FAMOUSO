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
