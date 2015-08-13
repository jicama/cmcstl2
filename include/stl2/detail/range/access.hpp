#ifndef STL2_DETAIL_RANGE_ACCESS_HPP
#define STL2_DETAIL_RANGE_ACCESS_HPP

#include <initializer_list>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/iterator/concepts.hpp>

// HACKHACK: Disable constraints that cause the compiler to OOM
#define STL2_HACK_END_CONSTRAINTS 1

namespace stl2 { inline namespace v1 {
BidirectionalIterator{I} class reverse_iterator;
BidirectionalIterator{I}
reverse_iterator<I> make_reverse_iterator(I);

///////////////////////////////////////////////////////////////////////////
// Range access [iterator.range]
//
// begin
// 20150805: Not to spec: is an N4381-style customization point.
namespace __begin {
  // Use member if it returns Iterator.
  template <class R>
    requires requires (R& r) {
      STL2_DEDUCTION_CONSTRAINT(r.begin(), Iterator);
    }
  constexpr auto impl(R& r, ext::priority_tag<0>)
    noexcept(noexcept(r.begin())) {
    return r.begin();
  }

  // Prefer ADL if it returns Iterator.
  template <class R>
    requires requires (R& r) {
      STL2_DEDUCTION_CONSTRAINT(begin(r), Iterator);
    }
  constexpr auto impl(R& r, ext::priority_tag<1>)
    noexcept(noexcept(begin(r))) {
    return begin(r);
  }

  struct fn {
    template <class R, std::size_t N>
    constexpr R* operator()(R (&array)[N]) const noexcept {
      return array;
    }

    template <class R>
      requires requires (R& r) { __begin::impl(r, ext::max_priority_tag); }
    constexpr auto operator()(R& r) const
      noexcept(noexcept(__begin::impl(r, ext::max_priority_tag))) {
      return __begin::impl(r, ext::max_priority_tag);
    }

#if 1
    // Backwards compatibility: rvalues are treated as
    // const lvalues.
    template <class R>
      requires requires (const R& r) { __begin::impl(r, ext::max_priority_tag); }
    constexpr auto operator()(const R&& r) const
      noexcept(noexcept(__begin::impl(r, ext::max_priority_tag))) {
      return __begin::impl(r, ext::max_priority_tag);
    }
#else
    // Forget backwards compatibility, begin on rvalues is broken;
    // except for std::initializer_list.
    template <class E>
    constexpr const E* operator()(std::initializer_list<E> il) const noexcept {
      return il.begin();
    }
#endif
  };
}
namespace {
  constexpr auto& begin = detail::static_const<__begin::fn>::value;
}

// end
namespace __end {
  // Use member if it returns Sentinel.
  template <class R>
    requires requires (R& r) {
      // { r.end() } -> Sentinel<decltype(stl2::begin(r))>;
      r.end();
#if !STL2_HACK_END_CONSTRAINTS
      requires Sentinel<decltype(r.end()), decltype(stl2::begin(r))>();
#endif
    }
  constexpr auto impl(R& r, ext::priority_tag<0>)
    noexcept(noexcept(r.end())) {
#if STL2_HACK_END_CONSTRAINTS
    static_assert(ext::models::sentinel<decltype(r.end()), decltype(stl2::begin(r))>());
#endif
    return r.end();
  }

  // Prefer ADL if it returns Sentinel.
  template <class R>
    requires requires (R& r) {
      // { end(r) } -> Sentinel<decltype(stl2::begin(r))>;
      end(r);
#if !STL2_HACK_END_CONSTRAINTS
      requires Sentinel<decltype(end(r)), decltype(stl2::begin(r))>();
#endif
    }
  constexpr auto impl(R& r, ext::priority_tag<1>)
    noexcept(noexcept(end(r))) {
#if STL2_HACK_END_CONSTRAINTS
    static_assert(ext::models::sentinel<decltype(end(r)), decltype(stl2::begin(r))>());
#endif
    return end(r);
  }

  struct fn {
    template <class R, std::size_t N>
    constexpr R* operator()(R (&array)[N]) const noexcept {
      return array + N;
    }

    template <class R>
      requires requires (R& r) { __end::impl(r, ext::max_priority_tag); }
    constexpr auto operator()(R& r) const
      noexcept(noexcept(__end::impl(r, ext::max_priority_tag))) {
      return __end::impl(r, ext::max_priority_tag);
    }

#if 1
    // Backwards compatibility: rvalues are treated as
    // const lvalues.
    template <class R>
      requires requires (const R& r) { __end::impl(r, ext::max_priority_tag); }
    constexpr auto operator()(const R&& r) const
      noexcept(noexcept(__end::impl(r, ext::max_priority_tag))) {
      return __end::impl(r, ext::max_priority_tag);
    }
#else
    // Forget backwards compatibility, end on rvalues is broken;
    // except for std::initializer_list.
    template <class E>
    constexpr const E* operator()(std::initializer_list<E> il) const noexcept {
      return il.end();
    }
#endif
  };
}
namespace {
  constexpr auto& end = detail::static_const<__end::fn>::value;
}

// cbegin
namespace __cbegin {
  struct fn {
    template <class R>
      requires requires (const R& r) { stl2::begin(r); }
    constexpr auto operator()(const R& r) const
      noexcept(noexcept(stl2::begin(r))) {
      return stl2::begin(r);
    }
  };
}
namespace {
  constexpr auto& cbegin = detail::static_const<__cbegin::fn>::value;
}

// cend
namespace __cend {
  struct fn {
    template <class R>
      requires requires (const R& r) { stl2::end(r); }
    constexpr auto operator()(const R& r) const
      noexcept(noexcept(stl2::end(r))) {
      return stl2::end(r);
    }
  };
}
namespace {
  constexpr auto& cend = detail::static_const<__cend::fn>::value;
}

// rbegin
namespace __rbegin {
  // Default to make_reverse_iterator(end(r)) for Bounded ranges of
  // Bidirectional iterators.
  template <class R>
    requires requires (R& r) {
      requires Same<decltype(stl2::begin(r)), decltype(stl2::end(r))>();
      make_reverse_iterator(stl2::end(r));
    }
  constexpr auto impl(R& r, ext::priority_tag<0>)
    noexcept(noexcept(make_reverse_iterator(stl2::end(r)))) {
    return make_reverse_iterator(stl2::end(r));
  }

  // Prefer member if it returns Iterator
  template <class R>
    requires requires (R& r) {
      // { r.rbegin() } -> Iterator;
      STL2_DEDUCTION_CONSTRAINT(r.rbegin(), Iterator);
    }
  constexpr auto impl(R& r, ext::priority_tag<1>)
    noexcept(noexcept(r.rbegin())) {
    return r.rbegin();
  }

  struct fn {
    template <class R>
      requires requires (R& r) { __rbegin::impl(r, ext::max_priority_tag); }
    constexpr auto operator()(R& r) const
      noexcept(noexcept(__rbegin::impl(r, ext::max_priority_tag))) {
      return __rbegin::impl(r, ext::max_priority_tag);
    }

#if 1
    // Backwards compatibility: rvalues are treated as
    // const lvalues.
    template <class R>
      requires requires (const R& r) { __rbegin::impl(r, ext::max_priority_tag); }
    constexpr auto operator()(const R&& r) const
      noexcept(noexcept(__rbegin::impl(r, ext::max_priority_tag))) {
      return __rbegin::impl(r, ext::max_priority_tag);
    }
#else
    // Forget backwards compatibility,
    // except for std::initializer_list.
    template <class E>
    constexpr reverse_iterator<const E*>
    operator()(std::initializer_list<E> il) const noexcept {
      return reverse_iterator<const E*>{il.end()};
    }
#endif
  };
}
namespace {
  constexpr auto& rbegin = detail::static_const<__rbegin::fn>::value;
}

// rend
#define STL2_HACK_REND_CONSTRAINTS STL2_HACK_END_CONSTRAINTS
namespace __rend {
  // Default to make_reverse_iterator(begin(r)) for Bounded ranges of
  // Bidirectional iterators.
  template <class R>
    requires requires (R& r) {
      requires Same<decltype(stl2::begin(r)), decltype(stl2::end(r))>();
      make_reverse_iterator(stl2::begin(r));
    }
  constexpr auto impl(R& r, ext::priority_tag<0>)
    noexcept(noexcept(make_reverse_iterator(stl2::begin(r)))) {
    return make_reverse_iterator(stl2::begin(r));
  }

  // Prefer member if it returns Sentinel
  template <class R>
    requires requires (R& r) {
      // { r.rend() } -> Sentinel<decltype(stl2::rbegin(r))>;
      r.rend();
#if !STL2_HACK_REND_CONSTRAINTS
      requires Sentinel<decltype(r.rend()), decltype(stl2::rbegin(r))>();
#endif
    }
  constexpr auto impl(R& r, ext::priority_tag<1>)
    noexcept(noexcept(r.rend())) {
#if STL2_HACK_REND_CONSTRAINTS
    static_assert(ext::models::sentinel<decltype(r.rend()), decltype(stl2::rbegin(r))>());
#endif
    return r.rend();
  }

  struct fn {
    template <class R>
      requires requires (R& r) { __rend::impl(r, ext::max_priority_tag); }
    constexpr auto operator()(R& r) const
      noexcept(noexcept(__rend::impl(r, ext::max_priority_tag))) {
      return __rend::impl(r, ext::max_priority_tag);
    }

#if 1
    // Backwards compatibility: rvalues are treated as
    // const lvalues.
    template <class R>
      requires requires (const R& r) { __rend::impl(r, ext::max_priority_tag); }
    constexpr auto operator()(const R&& r) const
      noexcept(noexcept(__rend::impl(r, ext::max_priority_tag))) {
      return __rend::impl(r, ext::max_priority_tag);
    }
#else
    // Forget backwards compatibility,
    // except for std::initializer_list.
    template <class E>
    constexpr reverse_iterator<const E*>
    operator()(std::initializer_list<E> il) const noexcept {
      return reverse_iterator<const E*>{il.begin()};
    }
#endif
  };
}
namespace {
  constexpr auto& rend = detail::static_const<__rend::fn>::value;
}

// crbegin
namespace __crbegin {
  struct fn {
    template <class R>
      requires requires (const R& r) { stl2::rbegin(r); }
    constexpr auto operator()(const R& r) const
      noexcept(noexcept(stl2::rbegin(r))) {
      return stl2::rbegin(r);
    }
  };
}
namespace {
  constexpr auto& crbegin = detail::static_const<__crbegin::fn>::value;
}

// crend
namespace __crend {
  struct fn {
    template <class R>
      requires requires (const R& r) { stl2::rend(r); }
    constexpr auto operator()(const R& r) const
      noexcept(noexcept(stl2::rend(r))) {
      return stl2::rend(r);
    }
  };
}
namespace {
  constexpr auto& crend = detail::static_const<__crend::fn>::value;
}

///////////////////////////////////////////////////////////////////////////
// Container access [iterator.container]
//
// size
namespace __size {
  template <class T, std::size_t N>
  constexpr std::size_t size(T(&)[N]) noexcept {
    return N;
  }

  template <class R>
    requires requires (const R& r) { r.size(); }
  constexpr auto size(const R& r)
    noexcept(noexcept(r.size())) {
    return r.size();
  }

  struct fn {
    template <class R>
      requires requires (const R& r) { size(r); }
    constexpr auto operator()(const R& r) const
      noexcept(noexcept(size(r))) {
      return size(r);
    }
  };
}
namespace {
  constexpr auto& size = detail::static_const<__size::fn>::value;
}

// empty
namespace __empty {
  template <class R>
    requires requires (const R& r) {
      STL2_CONVERSION_CONSTRAINT(r.empty(), bool);
    }
  constexpr bool impl(const R& r)
    noexcept(noexcept(bool(r.empty()))) {
    return r.empty();
  }

  struct fn {
    template <class T, std::size_t N>
    constexpr bool operator()(T(&)[N]) const noexcept {
      return N == 0;
    }

    template <class E>
    constexpr bool operator()(std::initializer_list<E> il) const noexcept {
      return il.size() == 0;
    }

    template <class R>
      requires requires (const R& r) { __empty::impl(r); }
    constexpr bool operator()(const R& r) const
      noexcept(noexcept(__empty::impl(r))) {
      return __empty::impl(r);
    }
  };
}
namespace {
  constexpr auto& empty = detail::static_const<__empty::fn>::value;
}

// data
namespace __data {
  template <class R>
    requires requires (R& r) {
      //{ r.data() } -> _Is<std::is_pointer>;
      requires _Is<decltype(r.data()), std::is_pointer>;
    }
  constexpr auto impl(R& r) noexcept(noexcept(r.data())) {
    return r.data();
  }

  struct fn {
    template <class T, std::size_t N>
    constexpr T* operator()(T (&array)[N]) const noexcept {
      return array;
    }

    template <class E>
    constexpr const E* operator()(std::initializer_list<E> il) const noexcept {
      return il.begin();
    }

    template <class R>
      requires requires (R& r) { __data::impl(r); }
    constexpr auto operator()(R& r) const
      noexcept(noexcept(__data::impl(r))) {
      return __data::impl(r);
    }

#if 1
    // Backwards compatibility: rvalues are treated as
    // const lvalues.
    template <class R>
      requires requires (const R& r) { __data::impl(r); }
    constexpr auto operator()(const R&& r) const
      noexcept(noexcept(__data::impl(r))) {
      return __data::impl(r);
    }
#else
    // Forget backwards compatibility, data on rvalues is broken;
    // except for std::initializer_list.
#endif
  };
}
namespace {
  constexpr auto& data = detail::static_const<__data::fn>::value;
}
}} // namespace stl2::v1

#endif
