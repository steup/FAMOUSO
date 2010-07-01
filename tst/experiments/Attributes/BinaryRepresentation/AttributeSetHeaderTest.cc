#include "logging/logging.h"

#include "AttribTests.h"

#include "mw/attributes/detail/AttributeSetHeader.h"

typedef famouso::mw::attributes::detail::AttributeSetHeader<0x01>   h1;
typedef famouso::mw::attributes::detail::AttributeSetHeader<0x7F>   h2;
typedef famouso::mw::attributes::detail::AttributeSetHeader<0xFF>   h3;
typedef famouso::mw::attributes::detail::AttributeSetHeader<0x7FFF> h4;
//typedef famouso::mw::attributes::detail::AttributeSetHeader<0xFFFF> h5; // Error: 0xFFFF exceeds format bounds

int main(int argc, char** argv) {
    h1 hd1;
    h2 hd2;
    h3 hd3;
    h4 hd4;

    DBG_MSG(hd1.get());
    DBG_MSG(hd2.get());
    DBG_MSG(hd3.get());
    DBG_MSG(hd4.get());

    hd1.set(0x7F);
    hd2.set(0xFF); // Should fail

    DBG_MSG(hd1.get()); // Should now show 127
    DBG_MSG(hd2.get()); // Should still show 127 (setting to 255 above should fail)
}
