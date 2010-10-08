#include <typeinfo>

#include "logging/logging.h"

#include "config/type_traits/if_contains_type.h"

#include "boost/type_traits/is_same.hpp"
#include "boost/mpl/eval_if.hpp"
#include "boost/mpl/less.hpp"
#include "boost/mpl/lambda.hpp"
#include "boost/mpl/vector_c.hpp"
#include "boost/mpl/bool.hpp"
#include "boost/mpl/integral_c.hpp"
#include "boost/mpl/deref.hpp"
#include "boost/mpl/begin.hpp"
#include "boost/mpl/end.hpp"
#include "boost/mpl/next.hpp"

#ifndef LIST
#warning No list defined, using default
typedef boost::mpl::vector_c<int, 10, 1, 2, 3, 4, 9>::type list;
#else
typedef LIST list;
#endif

typedef boost::mpl::less<boost::mpl::_1, boost::mpl::_2> pred;

IF_CONTAINS_TYPE_(tag);

template <typename T>
struct is_integral_wrapper {
        static const bool value = boost::mpl::eval_if<
                                               if_contains_type_tag<T>,
                                               boost::is_same<typename T::tag, boost::mpl::integral_c_tag>,
                                               boost::mpl::bool_<false>
                                              >::type::value;
};

template <typename S, typename I = typename boost::mpl::begin<S>::type>
struct printer {
        typedef typename boost::mpl::deref<I>::type elem;

        static void print() {
            if (is_integral_wrapper<elem>::value) {
                ::logging::log::emit() << elem::value << ", ";
            } else {
                ::logging::log::emit() << typeid(elem).name() << ", ";
            }

            printer<S, typename boost::mpl::next<I>::type>::print();
        }
};
template <typename S>
struct printer<S, typename boost::mpl::end<S>::type> {
        static void print() {
            ::logging::log::emit() << ::logging::log::endl;
        }
};
