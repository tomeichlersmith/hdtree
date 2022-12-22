#pragma once

#include "hdtree/Version.h"

namespace hdtree {

/**
 * empty struct for connecting a instance of Data
 * and the type it wraps
 *
 * This struct has no members or instance methods,
 * it is merely here to grant Data access to
 * the potentially private function `attach`.
 *
 * A class interested in being wrapped by Data
 * should declare this struct as its friend.
 * ```cpp
 * friend class hdtree::access;
 * ```
 */
struct access {
  /**
   * Connect the input types by attaching t
   * to do
   *
   * @tparam T type of class with attach method defined
   * @tparam D type of class to pass to attach
   * @param[in] t handle of class to call attach on
   * @param[in] d handle of class to pass into attach
   */
  template <typename T, typename D>
  static void connect(T& t, D& d) {
    t.attach(d);
  }
};  // access

}  // namespace hdtree
