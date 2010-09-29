#include "assert/assert.h"

static const bool globalErrorFlag = false;

// None of the "no_error..." and "no_warning..." messages should appear when compiling

// Every message with "error_..." should appear in an error context, every message with
//  "warning_..." should appear in a warning context

struct T {
        FAMOUSO_STATIC_ASSERT(true, no_warning_within_struct_context, (), false);
        FAMOUSO_STATIC_ASSERT(!globalErrorFlag, no_error_within_struct_context, (), true);

        FAMOUSO_STATIC_ASSERT(false, warning_within_struct_context, (), false);
        FAMOUSO_STATIC_ASSERT(globalErrorFlag, error_within_struct_context, (), true);
};

template <typename U>
struct V {
        FAMOUSO_STATIC_ASSERT(true && sizeof(U) != 0, no_warning_within_template_struct_context, (U), false);
        FAMOUSO_STATIC_ASSERT(!globalErrorFlag, no_error_within_template_struct_context, (U), true);

        FAMOUSO_STATIC_ASSERT(false && sizeof(U) == 0, warning_within_template_struct_context, (U), false);
        FAMOUSO_STATIC_ASSERT(globalErrorFlag, error_within_template_struct_context, (U), true);
};

V<T> v;

FAMOUSO_STATIC_ASSERT(true, no_warning_within_global_context, (), false);
FAMOUSO_STATIC_ASSERT(!globalErrorFlag, no_error_within_global_context, (), true);

FAMOUSO_STATIC_ASSERT(false, warning_within_global_context, (), false);
FAMOUSO_STATIC_ASSERT(globalErrorFlag, error_within_global_context, (), true);

int main() {
    FAMOUSO_STATIC_ASSERT(true, no_warning_within_function_context, (), false);
    FAMOUSO_STATIC_ASSERT(!globalErrorFlag, no_error_within_function_context, (), true);

    FAMOUSO_STATIC_ASSERT(false, warning_within_function_context, (), false);
    FAMOUSO_STATIC_ASSERT(globalErrorFlag, error_within_function_context, (), true);
}
