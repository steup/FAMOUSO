#include "logging/logging.h"

#define EMIT ::logging::log::emit()
#define ENDL ::logging::log::endl

#include <stdint.h>

#include "mw/attributes/TTL.h"
#include "mw/attributes/Latency.h"
#include "mw/attributes/Omission.h"
#include "mw/attributes/Bandwidth.h"
#include "mw/common/ExtendedEvent.h"
#include "mw/common/Subject.h"

#include "mw/attributes/detail/SetProvider.h"
#include "mw/attributes/detail/RequirementChecker.h"

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

typedef detail::SetProvider<TTL<2>, Latency<20>, Bandwidth<250> >::attrSet prov;
typedef detail::SetProvider<TTL<2>, Latency<20>, Bandwidth<210> >::attrSet req;

typedef detail::RequirementChecker<prov, req> checker;

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

    EMIT << (int) checker::result::value << ENDL;

    return (0);
}
