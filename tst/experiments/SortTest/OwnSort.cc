#include "Util.h"
#include "InsertionSort.h"

typedef sort<list, pred>::type sorted;

int main(int argc, char **argv) {
    printer<list>::print();
    printer<sorted>::print();
}
