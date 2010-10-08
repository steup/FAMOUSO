#include "Util.h"

#include "boost/mpl/sort.hpp"

typedef boost::mpl::sort<list, pred>::type sorted;

int main(int argc, char **argv) {
    printer<list>::print();
    printer<sorted>::print();
}

