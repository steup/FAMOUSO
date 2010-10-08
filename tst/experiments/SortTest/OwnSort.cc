#include "Util.h"

#include "boost/mpl/begin.hpp"
#include "boost/mpl/lambda.hpp"
#include "boost/mpl/deref.hpp"
#include "boost/mpl/eval_if.hpp"
#include "boost/mpl/next.hpp"
#include "boost/mpl/end.hpp"
#include "boost/mpl/vector.hpp"
#include "boost/mpl/insert.hpp"

template <typename T>
struct wrap_type {
        typedef T type;
};

// Finds the iterator where the given element should be inserted in the
//  given SORTED sequence depending on the given predicate
template <typename Seq, typename Elem, typename Pred, typename Iter = typename boost::mpl::begin<Seq>::type>
struct find_pos {
        // Assure that the given predicate can be applied
        typedef typename boost::mpl::lambda<Pred>::type pred;

        // The current element
        typedef typename boost::mpl::deref<Iter>::type curElem;

        // If the current element fits the given predicate return the current iterator,
        //  otherwise continue iteration recursively
        typedef typename boost::mpl::eval_if<
                                      typename pred::template apply<curElem, Elem>::type,

                                      wrap_type<Iter>,

                                      wrap_type<typename find_pos<Seq, Elem, Pred, typename boost::mpl::next<Iter>::type>::type>
                                     >::type type;
};
template <typename Seq, typename Elem, typename Pred>
struct find_pos<Seq, Elem, Pred, typename boost::mpl::end<Seq>::type> {
        // The whole vector has been iterated, so we return the end iterator
        //  as the insertion position
        typedef typename boost::mpl::end<Seq>::type type;
};

// Sorts the given sequence using an insertion sort algorithm
template <typename Seq, typename Pred, typename Iter = typename boost::mpl::begin<Seq>::type, typename Sorted = boost::mpl::vector<> >
struct sort {
        // The current element of the source sequence
        typedef typename boost::mpl::deref<Iter>::type curElem;

        // The iterator where the current element should be inserted
        typedef typename find_pos<Sorted, curElem, Pred>::type insertPos;

        // The list with the inserted element
        typedef typename boost::mpl::insert<Sorted, insertPos, curElem>::type inserted;

        // The recursive instantiation for the next element
        typedef typename sort<Seq, Pred, typename boost::mpl::next<Iter>::type, inserted>::type type;
};
template <typename Seq, typename Pred, typename Sorted>
struct sort<Seq, Pred, typename boost::mpl::end<Seq>::type, Sorted> {
        // In the end case the given list is the result
        typedef Sorted type;
};

typedef sort<list, pred>::type sorted;

int main(int argc, char **argv) {
    printer<list>::print();
    printer<sorted>::print();
}
