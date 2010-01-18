/*******************************************************************************
 *
 * Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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
#define CPU_FREQUENCY 16000000UL

#include <stdio.h>
#include <stdlib.h>
#include "mw/common/Event.h"
#include "case/Delegate.h"

#include "debug.h"

#include "mw/attributes/EmptyAttribute.h"
// test the equality of types
template < typename T, typename U>
struct is_same{
    enum {value=0};
};

template < typename T>
struct is_same<T,T>{
    enum {value=1};
};

using namespace famouso::mw::attributes;

// defines a configurable Time-To-Life attribute
template<uint8_t ttl>
class TTL : public EmptyAttribute {
    uint8_t data[size];
public:
    typedef TTL<0>  tag_type;
    typedef TTL type;
    enum {
        size = 2,
        value= ttl,
        id='T'
    };

    TTL() {
        data[0]=id;
        data[1]=value;
    }

    uint8_t get() { return data[1];}
    void set(uint8_t tmp) { data[1]=tmp;}
};

// some definition for convenience
typedef TTL<0> Local;
typedef TTL<1> Body;
typedef TTL<255> World;

// definition of an extended attribute
// the binary representation is as follows
// |C-AB|0..255 A|EP|
// C-AB -- number of attribute bytes
// A    -- the attributes in binary representation
// EP   -- event data/payload
//
// \tparam s is the size of the event payload
// \tparam Attr is an attribute attached to the event
//
template<uint16_t s=0, typename Attr=EmptyAttribute>
class ExtendedEvent : public famouso::mw::Event {
  public:
    enum {
        attr_length=Attr::size,
        local = is_same< Attr, Local >::value
    };

  private:
    // the whole event with attributes and payload
    uint8_t _edata[1+attr_length+s];

  public:
    ExtendedEvent (const famouso::mw::Subject &sub) : Event(sub) {
        _edata[0]=attr_length;
        new (&_edata[1]) Attr;
        length=s+1+attr_length;
        data=_edata;
    }

    // payload setting as simple as possible
    void operator = (const char* str) {
        uint16_t i=0;
        while( str[i] && (i<s)) {
            _edata[1+attr_length+i]=str[i];
            ++i;
        }
    }

    // try to check if the event contains the attribute <A> and
    // whether it has the right value
    template<typename A, typename comperator>
    bool check(){
        if ( (attr_length!=0) && (A::id == _edata[1]) )
            return comperator::apply(_edata[2], static_cast<uint8_t>(A::value));
        else
           return false;
    }

};


// try to find if the message contains the attribute <A>
template<typename A>
A* find(const famouso::mw::Event& e) {
    uint8_t *d=e.data;
    if (d[0] && (d[1] == A::tag_type::id))
        return reinterpret_cast<A*>(&d[1]);
    else
        return reinterpret_cast<A*>(NULL);
}

// helper template for comparing to types at run-time
struct compare_equality {
    template <typename T, typename U>
    static bool apply(T x, U y) {
        return !!(x==y);
    }
};

template< typename A>
struct has_ {
    template< typename t>
    static bool apply(t e) {
        return !!(find<A>(e));
    }
};

template< typename A>
struct not_ {
    template< typename t>
    static bool apply(t e) {
        return !(A::apply(e));
    }
};

struct true_ {
    template< typename t>
    static bool apply(t) {
        return true;
    }
};

struct false_ {
    template< typename t>
    static bool apply(t) {
        return false;
    }
};

template< typename A, typename B, typename C=true_, typename D=true_, typename E=true_>
struct and_ {
    template< typename t>
    static bool apply(t e) {
        return (A::apply(e) && B::apply(e) && C::apply(e) && D::apply(e) && E::apply(e));
    }
};

template< typename A, typename B, typename C=false_, typename D=false_, typename E=false_>
struct or_ {
    template< typename t>
    static bool apply(t e) {
        return (A::apply(e) || B::apply(e) || C::apply(e) || D::apply(e) || E::apply(e));
    }
};

template< typename A>
struct ge_ {
    template< typename t>
    static bool apply(t e) {
        A* a=find<A>(e);
        return !!(a && (a->get() >= A::value));
    }
};

template< typename A>
struct g_ {
    template< typename t>
    static bool apply(t e) {
        A* a=find<A>(e);
        return !!(a && (a->get() > A::value));
    }
};

template< typename A>
struct l_ {
    template< typename t>
    static bool apply(t e) {
        A* a=find<A>(e);
        return !!(a && (a->get() < A::value));
    }
};

template< typename A>
struct le_ {
    template< typename t>
    static bool apply(t e) {
        A* a=find<A>(e);
        return !!(a && (a->get() <= A::value));
    }
};


template < typename f>
struct filter {
    template< typename t>
    bool operator() (t e) {
        return f::apply(e);
    }

    template< typename t>
    static bool apply(t e) {
        return f::apply(e);
    }
};

template< typename F, typename T>
bool filter_and_transform(F& f, T e) {
    bool b=f(e);
    if ( b )
        ::logging::log::emit() << "Filter matches" << ::logging::log::endl;
    else
        ::logging::log::emit() << "Filter does not match" << ::logging::log::endl;

    return b;
}

int main(int argc, char **argv) {
    typedef filter< has_<Body> > has_body;
    typedef filter<
        and_<
           has_body,
//           has_<
//               Body
//               >,
            ge_<
                TTL<1>
               >,
            le_<
                TTL<2>
               >
            >
         > ftype;
    ftype f;

    famouso::util::Delegate<const famouso::mw::Event&, bool> d;
    d.bind<&has_body::apply>();

    // try different tests with the filter
    const ExtendedEvent<0, TTL<0> > e0(famouso::mw::Subject(0xf1));
    filter_and_transform(f,e0);
//    filter_and_transform(d,e0);

    ExtendedEvent<0, TTL<1> > e1(famouso::mw::Subject(0xf1));
    filter_and_transform(f,e1);

    ExtendedEvent<0, TTL<2> > e2(famouso::mw::Subject(0xf1));
    filter_and_transform(f,e2);

    ExtendedEvent<0, TTL<3> > e3(famouso::mw::Subject(0xf1));
    filter_and_transform(f,e3);

    ExtendedEvent<0 > e4(famouso::mw::Subject(0xf1));
    filter_and_transform(f,e4);

    return 0;
}
