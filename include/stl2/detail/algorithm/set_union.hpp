// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_SET_UNION_HPP
#define STL2_DETAIL_ALGORITHM_SET_UNION_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/tuple.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// set_union [set.union]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2,
            WeaklyIncrementable O, class Comp = less<>,
            class Proj1 = identity, class Proj2 = identity>
    requires Mergeable<I1, I2, O, Comp, Proj1, Proj2>()
  tagged_tuple<tag::in1(I1), tag::in2(I2), tag::out(O)>
  set_union(I1 first1, S1 last1, I2 first2, S2 last2, O result, Comp comp_ = Comp{},
            Proj1 proj1_ = Proj1{}, Proj2 proj2_ = Proj2{}) {
    auto&& comp = __stl2::as_function(comp_);
    auto&& proj1 = __stl2::as_function(proj1_);
    auto&& proj2 = __stl2::as_function(proj2_);

    while (true) {
      if (first1 == last1) {
        auto res = __stl2::copy(__stl2::move(first2), __stl2::move(last2), __stl2::move(result));
        return {__stl2::move(first1), __stl2::move(res.in()), __stl2::move(res.out())};
      }
      if (first2 == last2) {
        auto res = __stl2::copy(__stl2::move(first1), __stl2::move(last1), __stl2::move(result));
        return {__stl2::move(res.in()), __stl2::move(first2), __stl2::move(res.out())};
      }
      if (comp(proj1(*first1), proj2(*first2))) {
        *result = *first1;
        ++first1;
      } else {
        if (!comp(proj2(*first2), proj1(*first1))) {
          ++first1;
        }
        *result = *first2;
        ++first2;
      }
      ++result;
    }
  }

  template <InputRange Rng1, InputRange Rng2, WeaklyIncrementable O,
            class Comp = less<>, class Proj1 = identity, class Proj2 = identity>
    requires Mergeable<IteratorType<Rng1>, IteratorType<Rng2>, O, Comp, Proj1, Proj2>()
  tagged_tuple<tag::in1(safe_iterator_t<Rng1>), tag::in2(safe_iterator_t<Rng2>),
               tag::out(O)>
  set_union(Rng1&& rng1, Rng2&& rng2, O result, Comp&& comp = Comp{},
            Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    return __stl2::set_union(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::end(rng2),
      __stl2::move(result),
      __stl2::forward<Comp>(comp),
      __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }
} STL2_CLOSE_NAMESPACE

#endif