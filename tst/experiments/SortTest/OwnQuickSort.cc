#include "Util.h"

#include "boost/mpl/vector.hpp"
#include "boost/mpl/vector_c.hpp"
#include "boost/mpl/begin.hpp"
#include "boost/mpl/end.hpp"
#include "boost/mpl/deref.hpp"
#include "boost/mpl/eval_if.hpp"
#include "boost/mpl/lambda.hpp"
#include "boost/mpl/push_back.hpp"
#include "boost/mpl/next.hpp"
#include "boost/mpl/pop_front.hpp"
#include "boost/mpl/size.hpp"

template <typename Seq, typename Pivot, typename Pred, bool invert, typename Iter = typename boost::mpl::begin<Seq>::type, typename Result = boost::mpl::vector<> >
struct divide {
        typedef typename boost::mpl::lambda<Pred>::type pred;

        typedef typename boost::mpl::deref<Iter>::type curElem;

        static const bool result = pred::template apply<Pivot, curElem>::type::value;

        typedef typename boost::mpl::eval_if_c<
                                      invert ? !result : result,

                                      Result,

                                      boost::mpl::push_back<Result, curElem>
                                     >::type tmpResult;

        typedef typename divide<Seq, Pivot, Pred, invert, typename boost::mpl::next<Iter>::type, tmpResult>::type type;

};
template <typename Seq, typename Pivot, typename Pred, bool invert, typename Result>
struct divide<Seq, Pivot, Pred, invert, typename boost::mpl::end<Seq>::type, Result> {
        typedef Result type;
};

template <typename Seq, typename Pred, int sizeDummy = boost::mpl::size<Seq>::value>
struct quick_sort {
        typedef typename boost::mpl::deref<typename boost::mpl::begin<Seq>::type>::type pivot;

        typedef typename boost::mpl::pop_front<Seq>::type withoutPivot;

        typedef typename divide<withoutPivot, pivot, Pred, false>::type first;
        typedef typename divide<withoutPivot, pivot, Pred, true>::type second;

        typedef typename quick_sort<first, Pred>::type sorted1;
        typedef typename quick_sort<second, Pred>::type sorted2;

        typedef typename boost::mpl::push_back<sorted1, pivot>::type tmpResult;

        typedef typename merge<tmpResult, sorted2>::type type;
};
template <typename Seq, typename Pred>
struct quick_sort<Seq, Pred, 0> {
        typedef boost::mpl::vector<> type;
};

typedef quick_sort<list, pred>::type sorted;

int main(int argc, char **argv) {
    printer<list>::print();
    printer<sorted>::print();
}
