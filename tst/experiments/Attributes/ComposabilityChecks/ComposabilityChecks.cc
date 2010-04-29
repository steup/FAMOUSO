#include "logging/logging.h"

#include "mw/attributes/AttributeSequence.h"
#include "mw/attributes/detail/IsSameAttribute.h"
#include "mw/attributes/detail/FindStatic.h"
#include "mw/attributes/TTL.h"
#include "mw/attributes/tags/IntegralConstTag.h"

#include "boost/mpl/end.hpp"
#include "boost/mpl/deref.hpp"
#include "boost/mpl/vector.hpp"
#include "boost/mpl/list.hpp"
#include "boost/mpl/placeholders.hpp"
#include "boost/mpl/find_if.hpp"

#include "typeinfo"

using namespace famouso::mw::attributes;
using namespace boost;
using namespace boost::mpl;

#define EMIT ::logging::log::emit()
#define ENDL ::logging::log::endl

typedef TTL<0> ttl0;
typedef TTL<1> ttl1;

template <uint16_t delay>
struct Delay : public Attribute<Delay<0>, tags::integral_const_tag, uint16_t, delay, 1, true> {
    typedef Delay type;
};

typedef Delay<30> delay30;

template <uint16_t jitter>
struct Jitter : public Attribute<Jitter<0>, tags::integral_const_tag, uint16_t, jitter, true> {
    typedef Jitter type;
};


typedef list<ttl1,
             delay30,
             Jitter<1> > attrList;

AttributeSequence<attrList> seq;

int main(int argc, char* args[]) {

    return (0);
}
