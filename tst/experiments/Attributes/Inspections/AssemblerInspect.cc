#include <stdint.h>

#include "mw/attributes/Attribute.h"
#include "mw/attributes/tags/IntegralConstTag.h"

#include "mw/attributes/Latency.h"
#include "mw/attributes/TimeStamp.h"

using namespace famouso::mw::attributes;

template <uint32_t val>
class LD_Attr : public Attribute<LD_Attr<0>,  tags::integral_const_tag, uint32_t, val, 90> {
    typedef LD_Attr type;
};

template <typename Attr>
uint32_t getValue() {
    Attr attr;

    uint32_t v = attr.getValue();

    return (v);
}

int main(int argc, char **argv) {
    uint32_t result = 0;

    result += getValue<Latency<0> >();

    result += getValue<Latency<10> >();

    // A8 05 01 FF FF FF FF
    //result += getValue<TimeStamp<0x1FFFFFFFFULL> >();

    //result += getValue<TimeStamp<0x9> >();

    result += getValue<LD_Attr<10> >();

    return (result);
}
