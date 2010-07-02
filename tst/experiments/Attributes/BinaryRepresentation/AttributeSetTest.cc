#include "logging/logging.h"

#include "boost/mpl/vector.hpp"

#include "mw/attributes/TTL.h"

#include "mw/attributes/AttributeSet.h"

typedef boost::mpl::vector<famouso::mw::attributes::TTL<0> > vec1;

typedef famouso::mw::attributes::AttributeSet<vec1> set1;
typedef famouso::mw::attributes::AttributeSet<> set2;

int main(int argc, char** argv) {
    set1 s1;
    set2 s2;

    ::logging::log::emit() << set1::overallSize << " <-> " << sizeof(s1) << ::logging::log::endl;
    ::logging::log::emit() << set2::overallSize << " <-> " << sizeof(s2) << ::logging::log::endl;
}
