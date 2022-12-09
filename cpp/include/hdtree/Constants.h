/**
 * @file Constants.h
 * Definitions for names of structures required by
 * serialization methods.
 */
#ifndef FIRE_IO_CONSTANTS_H
#define FIRE_IO_CONSTANTS_H

#include <string>

namespace fire::io {

/**
 * Structure constants vital to serialization method
 */
struct constants {
  /// the name of the event header data set
  inline static const std::string EVENT_HEADER_NAME = "EventHeader";
  /// the name of the variable in event and run headers corresponding to their ID
  inline static const std::string NUMBER_NAME = "number";
  /// the name of the group in the file holding all event objects
  inline static const std::string EVENT_GROUP = "events";
  /// the name of the group holding the run headers
  inline static const std::string RUN_HEADER_NAME = "runs";
  /// the name of the HDF5 object attribute that holds the event object type
  inline static const std::string TYPE_ATTR_NAME = "__type__";
  /// the name of the fire version attribute
  inline static const std::string VERS_ATTR_NAME = "__version__";
  /// the name of the size dataset for variable types
  inline static const std::string SIZE_NAME = "__size__";
};

}

#endif
