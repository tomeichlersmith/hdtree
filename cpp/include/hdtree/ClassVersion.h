/**
 * @file ClassVersion.h
 * Serialization class version deduction
 */

#pragma once

namespace hdtree {

/**
 * hide the SFINAE confusing nonsense from the rest of the world
 *
 * reader beware! This stuff is **funky** to look at.
 */
namespace class_version_impl {

/**
 * Check if the passed type T is a valid type at compile time
 *
 * Copied from https://stackoverflow.com/a/9644512
 *
 * This is necessary so we can check if the compile-time defined
 * version number (stored as a type) is defined for a specific class.
 */
template <class T, class R = void>
struct enable_if_type {
  using type = R;
};

/**
 * Underlying struct deducing the version of a class
 *
 * This base struct holds the default version for any class
 * not matching the specialization below. It sets the default
 * version number to zero.
 *
 * @tparam[in] T class to deduce version for
 * @tparam[in] Enable non-void to go to specialization below
 */
template <class T, class Enable = void>
struct deducer {
  using version = std::integral_constant<int, 0>;
};

/**
 * Underlying struct deducing the version of a class
 *
 * This specialization matches any class `T` which defines
 * the subtype `T::version`.
 *
 * @note We do not check that `T::version` is correctly defined
 * to be an std::integral_constant.
 *
 * @tparam[in] T class to deduce version for
 */
template <class T>
struct deducer<T, typename enable_if_type<typename T::version>::type> {
  using version = typename T::version;
};

}  // namespace class_version_impl

/**
 * Helper const expression to pull out class version number
 *
 * **Assuming T::version is an std::integral_constant (same as the
 * default template struct)**, we simply pull out that integral_constant
 * value from class_version_impl::deducer.
 *
 * @tparam[in] T class to deduce version for
 */
template <typename T>
inline constexpr int class_version =
    class_version_impl::deducer<T>::version::value;

}  // namespace hdtree

/**
 * define the version number for a class
 *
 * Put this macro within the class declaration in order to define
 * its version number as a class used for data serialization in fire.
 * Without this macro, the version will default to zero.
 */
#define fire_class_version(VERS) \
  using version = std::integral_constant<int, VERS>
