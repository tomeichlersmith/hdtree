/** @file Atomic.h */

#pragma once

/**
 * Geant4 does a GLOBAL definition of the keyword TRUE
 */
#ifdef TRUE
#define NEED_TO_REDEFINE_TRUE
#undef TRUE
#undef FALSE
#endif

#include <type_traits>

#include <highfive/H5DataType.hpp>

namespace hdtree {

/**
 * HighFive supports many C++ "atomic" types that are used regularly.
 * In order to conform to our more flexible structure, I have isolated
 * their deduction into this (currently small) header file.
 *
 * This will make it easier to fold in future atomic types if we see fit.
 */
template <typename AtomicType>
using is_atomic =
    std::integral_constant<bool,
                           std::is_arithmetic<AtomicType>::value ||
                               std::is_same<AtomicType, std::string>::value>;

/// shorthand for easier use
template <typename AtomicType>
inline constexpr bool is_atomic_v = is_atomic<AtomicType>::value;

/**
 * Boolean enum aligned with h5py
 *
 * We serialize bools in the same method as h5py so that
 * Python-based analyses are easier.
 */
enum class Bool : bool {
  TRUE  = true,
  FALSE = false
};

/**
 * HighFive method for creating the enum data type
 */
HighFive::EnumType<Bool> create_enum_bool();

}  // namespace hdtree

/**
 * full specialization of HighFive template function
 *
 * This is necessary for compile-time registration of a new type
 * and is usually hidden within the HIGHFIVE_REGISTER_TYPE macro.
 *
 * The reason we have to _not_ use the macro here is two fold.
 * 1. We need the declaration of this registration to be done in this header 
 *    so that it is accessible by both the hdtree target and the downstream
 *    fire::framework target.
 * 2. We need to define the registration only once to avoid a duplicate
 *    definition compiler error during the linking step.
 *
 * These two goals can be met by splitting the declaration and the 
 * definition of this full specialization into the header and source
 * files of Atomic.
 */
template<>
HighFive::DataType HighFive::create_datatype<hdtree::Bool>();

