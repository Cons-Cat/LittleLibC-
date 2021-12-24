// -*- mode: c++ -*-
// vim: set ft=cpp:
#pragma once

#include <type_traits.h>
#include <utility.h>

/* To enhance legibility, I have prefixed type traits with std::, whereas
 * concepts have no prefix. */

// TODO: Remove usage of type_traits as much as possible.
namespace meta {

template <typename T, typename U>
concept same_as = meta::is_same_v<T, U>;

template <typename T>
concept integral = meta::is_integral_v<T>;

template <typename T>
concept signed_integral = integral<T> && meta::is_signed_v<T>;

template <typename T>
concept unsigned_integral = integral<T> && meta::is_unsigned_v<T>;

template <typename T>
concept floating_point = !integral<T> && meta::is_floating_point_v<T>;

template <typename T>
concept int_or_float = integral<T> || floating_point<T>;

// TODO: meta::is_safe_integral<T> does not work.
template <typename T>
concept safe_integral =
    (!meta::is_integral_v<T>)&&(!meta::is_floating_point_v<T>);

template <typename Derived, typename Base>
concept derived_from = meta::is_base_of_v<Base, Derived> &&
    meta::is_convertible_v<const volatile Derived*, const volatile Base*>;

template <typename From, typename To>
concept convertible_to = meta::is_convertible_v<From, To> && requires {
    static_cast<To>(meta::declval<From>());
};

template <typename T, typename... Args>
concept constructible_from = meta::is_constructible_v<T, Args...>;

template <typename T>
concept move_constructible = constructible_from<T, T> && convertible_to<T, T>;

template <typename T>
concept copy_constructible = move_constructible<T> &&
    constructible_from<T, T&> && convertible_to<T&, T> &&
    constructible_from<T, T const&> && convertible_to<T const&, T> &&
    constructible_from<T, T const> && convertible_to<T const, T>;

/* boolean_testable is adapted from the exposition-only concept
   boolean-testable. */
namespace detail {
    template <typename T>
    concept boolean_testable = convertible_to<T, bool>;
}

template <typename T>
concept boolean_testable = detail::boolean_testable<T> && requires(T&& b) {
    { !forward<T>(b) } -> detail::boolean_testable;
};

/* boolean_testable is adapted from the exposition-only concept
   __WeaklyEqualityComparableWith. */
namespace detail {
    template <typename T, typename U>
    concept weakly_equality_comparable_with =
        requires(meta::remove_reference_t<T> const& t,
                 meta::remove_reference_t<U> const& u) {
        { t == u } -> boolean_testable;
        { t != u } -> boolean_testable;
        { u == t } -> boolean_testable;
        { u != t } -> boolean_testable;
    };
}  // namespace detail

template <typename T>
concept equality_comparable = detail::weakly_equality_comparable_with<T, T>;

template <typename T>
concept enum_class = meta::is_scoped_enum_v<T>;

template <typename T, typename U>
concept narrow_convertible = requires() {
    U({meta::declval<T>()});
};

/* Some concepts from the STL are not supported, for various reasons.
 * `std::destructable` is not useful without exception handling. */

}  // namespace meta
