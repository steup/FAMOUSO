#include "logging/logging.h"

#define EMIT ::logging::log::emit()
#define ENDL ::logging::log::endl

#include <typeinfo>
#include <stdint.h>

#include "mw/attributes/TTL.h"
#include "mw/attributes/Latency.h"
#include "mw/attributes/Omission.h"
#include "mw/attributes/Throughput.h"
#include "mw/common/ExtendedEvent.h"
#include "mw/common/Subject.h"

#include "mw/attributes/detail/AttributeSetProvider.h"
#include "mw/attributes/detail/RequirementChecker.h"

#include "mw/attributes/detail/tags/TagSet.h"

#include "boost/mpl/vector.hpp"
#include "boost/mpl/list.hpp"

using namespace famouso::mw::attributes;

typedef boost::mpl::vector<TTL<1>, Latency<100>, Omission<10> > vec;
typedef boost::mpl::list<TTL<1>, Latency<100>, Omission<10> >   lis;

typedef famouso::mw::ExtendedEvent<16, vec> ev1; // OK -> vec is used as the sequence
typedef famouso::mw::ExtendedEvent<16, lis> ev2; // OK -> lis is used as the sequence

typedef famouso::mw::ExtendedEvent<16, TTL<1> > ev3; // OK -> a sequence with only TTL<1> is constructed
typedef famouso::mw::ExtendedEvent<16, TTL<1>, Latency<100> > ev4; // OK -> a sequence with 2 attributes is constructed
typedef famouso::mw::ExtendedEvent<16, TTL<1>, Latency<100>, Omission<10> > ev5; // OK -> a sequence with 3 attributes is constructed

typedef famouso::mw::ExtendedEvent<16, vec, lis> ev6; // error -> more than one type given and not all are attributes

typedef famouso::mw::ExtendedEvent<16, TTL<1>, TTL<2> > ev7; // error -> duplicate attributes

typedef famouso::mw::ExtendedEvent<> ev8; // OK -> no attribute is given at all

typedef detail::SetProvider<TTL<2>, Latency<10>, Throughput<310> >::attrSet prov;
typedef detail::SetProvider<TTL<2>, Latency<20>, Throughput<310> >::attrSet req;

typedef detail::RequirementChecker<prov, req, true> checker;

typedef detail::TagSet<>::type tagset;


typedef boost::mpl::vector<Omission<3>, TTL<1>, Throughput<20>, Latency<150> >::type toSort;

typedef detail::SortedAttributeSequence<toSort>::result::type sorted;

template <typename S, typename I = typename boost::mpl::begin<S>::type>
struct printer {
        static void print() {
            EMIT << typeid(typename boost::mpl::deref<I>::type).name() << ", ";

            printer<S, typename boost::mpl::next<I>::type>::print();
        }
};
template <typename S>
struct printer<S, typename boost::mpl::end<S>::type> {
        static void print() {
            EMIT << ENDL;
        }
};

template <typename S>
struct Babel {
        Babel() {
            int a;
        }
};

int main(int argc, char* args[]) {
    famouso::mw::Subject s(0x01);

    ev1 e1(s);
    ev2 e2(s);

    ev3 e3(s);
    ev4 e4(s);
    ev5 e5(s);

//    ev6 e6(s); // Here an error occurs

//    ev7 e7(s); // Here another error occurs

    ev8 e8(s);

    EMIT << (int) checker::value << ENDL;

    printer<toSort>::print();
    printer<sorted>::print();

    Babel<toSort> b1;
    Babel<sorted> b2;

    return (0);
}
