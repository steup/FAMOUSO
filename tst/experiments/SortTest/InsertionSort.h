#ifndef _INSERTION_SORT_H_
#define _INSERTION_SORT_H_

#include "Util.h"

#include "boost/mpl/begin.hpp"
#include "boost/mpl/lambda.hpp"
#include "boost/mpl/deref.hpp"
#include "boost/mpl/eval_if.hpp"
#include "boost/mpl/next.hpp"
#include "boost/mpl/end.hpp"
#include "boost/mpl/vector.hpp"
#include "boost/mpl/push_back.hpp"

// Inserts an element into the given sequence at the correct position depending on the given predicate
template <typename Seq, typename Elem, typename Pred, typename Iter = typename boost::mpl::begin<Seq>::type, typename Result = boost::mpl::vector<> >
struct insert_at {
        // The predicate as an applicable lambda struct
        typedef typename boost::mpl::lambda<Pred>::type pred;

        // The current element of the source list
        typedef typename boost::mpl::deref<Iter>::type curElem;

        // Definition for the case that the predicate evaluates to true

        // The result list with the element to insert inserted at the tail
        typedef typename boost::mpl::push_back<Result, Elem>::type tmpSrc;

        // The current result list merged with the rest of the source list (The current
        //  iterator is given to the merge struct here)
        typedef merge<tmpSrc, Seq, Iter> tmpMerged;

        // ---

        // Definition for the case that the predicate evaluates to false

        // The result list with the current element inserted at the tail
        typedef typename boost::mpl::push_back<Result, curElem>::type tmpResult;

        // The recursive call for the next source list element
        typedef insert_at<Seq, Elem, Pred, typename boost::mpl::next<Iter>::type, tmpResult> nextInst;

        // ---

        typedef typename boost::mpl::eval_if<
                                       typename pred::template apply<Elem, curElem>::type,

                                       tmpMerged,

                                       nextInst
                                      >::type type;
};
template <typename Seq, typename Elem, typename Pred, typename Result>
struct insert_at<Seq, Elem, Pred, typename boost::mpl::end<Seq>::type, Result> {
        typedef typename boost::mpl::push_back<Result, Elem>::type type;
};

// Sorts the given sequence using an insertion sort algorithm
template <typename Seq, typename Pred, typename Iter = typename boost::mpl::begin<Seq>::type, typename Sorted = boost::mpl::vector<> >
struct sort {
        // The current element of the source sequence
        typedef typename boost::mpl::deref<Iter>::type curElem;

        // The destination list with the current element inserted at the correct position
        typedef typename insert_at<Sorted, curElem, Pred>::type inserted;

        // The recursive instantiation for the next element
        typedef typename sort<Seq, Pred, typename boost::mpl::next<Iter>::type, inserted>::type type;
};
template <typename Seq, typename Pred, typename Sorted>
struct sort<Seq, Pred, typename boost::mpl::end<Seq>::type, Sorted> {
        // In the end case the given list is the result
        typedef Sorted type;
};

#endif // _INSERTION_SORT_H_
