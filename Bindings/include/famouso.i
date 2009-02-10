%module famouso
%include <inttypes.i>
%include <stdint.i>
%{
#include "famouso_bindings.h"
%}

#ifdef SWIGJAVA
// swig/JAVA does not natively support returning byte arrays; implement special case
// still TODO: only one function for data + len - take over the argument!
%include <various.i>
%apply char* BYTE {char* data}
%typemap(out, noblock=1) char *data {
    if (arg1->data) {
        $result = JCALL1(NewByteArray, jenv, arg1->len);
        JCALL4(SetByteArrayRegion, jenv, $result, 0, arg1->len, (const jbyte*)arg1->data);
    }
}
#endif

%include "famouso_bindings.h"

