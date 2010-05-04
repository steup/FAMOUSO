#include "logging/logging.h"

#define EMIT ::logging::log::emit()
#define ENDL ::logging::log::endl

#include <stdint.h>

#include "mw/attributes/TTL.h"
#include "mw/common/ExtendedEvent.h"
#include "mw/common/Subject.h"

#include "boost/mpl/vector.hpp"
#include "boost/mpl/list.hpp"


using namespace famouso::mw::attributes;

struct Delay : public Attribute<Delay, tags::integral_const_tag, bool, true, 9, true> {
    typedef Delay type;
};

struct Omission : public Attribute<Omission, tags::integral_const_tag, bool, true, 10, true> {
    typedef Omission type;
};

typedef boost::mpl::vector<TTL<1>, Delay, Omission> vec;
typedef boost::mpl::list<TTL<1>, Delay, Omission>   lis;

typedef famouso::mw::ExtendedEvent<16, vec> ev1; // OK -> vec is used as the sequence
typedef famouso::mw::ExtendedEvent<16, lis> ev2; // OK -> lis is used as the sequence

typedef famouso::mw::ExtendedEvent<16, TTL<1> > ev3; // OK -> a sequence with only TTL<1> is constructed
typedef famouso::mw::ExtendedEvent<16, TTL<1>, Delay > ev4; // OK -> a sequence with 2 attributes is constructed
typedef famouso::mw::ExtendedEvent<16, TTL<1>, Delay, Omission > ev5; // OK -> a sequence with 3 attributes is constructed

typedef famouso::mw::ExtendedEvent<16, vec, lis> ev6; // error -> more than one type given and not all are attributes

int main(int argc, char* args[]) {
    famouso::mw::Subject s(0x01);

    ev1 e1(s);
    ev2 e2(s);

    ev3 e3(s);
    ev4 e4(s);
    ev5 e5(s);

//    ev6 e6(s); // Here the error occurrs

    return (0);
}
