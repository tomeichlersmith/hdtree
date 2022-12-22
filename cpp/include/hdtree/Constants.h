/**
 * @file Constants.h
 * Definitions for names of structures required by
 * serialization methods.
 */
#pragma once

#include <string>

namespace hdtree {

/**
 * Structure constants vital to serialization method
 */
struct constants {
  /// the name of the HDF5 object attribute that holds the event object type
  inline static const std::string TYPE_ATTR_NAME = "__type__";
  /// the name of the hdtree version attribute
  inline static const std::string VERS_ATTR_NAME = "__version__";
  /// the name of the size dataset for variable-length types
  inline static const std::string SIZE_NAME = "__size__";
};

}  // namespace hdtree
