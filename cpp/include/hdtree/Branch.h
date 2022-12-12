#pragma once

#include <memory>
#include <type_traits>
#include <vector>
#include <map>
#include <exception>

#include "hdtree/Access.h"
#include "hdtree/Writer.h"
#include "hdtree/Constants.h"
#include "hdtree/Reader.h"
#include "hdtree/AbstractBranch.h"

/**
 * serialization to and from HDF5 files
 *
 * Isolation of lower-level interaction with HDF5 files is done here.
 * These classes should never be used directly by the end user except
 * for the attaching-method of defining new serializable types as 
 * described in [Getting Started](md_docs_getting_started.html).
 *
 * ## Access Pattern
 *
 * Below is a sketch of how the various hdtree::Branch template classes
 * interact with each other and the broader fire ecosystem.
 *
 * @image html fire_io_Data_AccessPattern.svg
 */
namespace hdtree {

// need implementation here so we can use the fully defined input file `type` function
template <typename DataType>
AbstractBranch<DataType>::AbstractBranch(const std::string& path, Reader* input_file, DataType* handle)
  : BaseBranch(path), owner_{handle == nullptr} {
  if (owner_) {
    handle_ = new DataType;
  } else {
    handle_ = handle;
  }
  save_type_ = { boost::core::demangle(typeid(DataType).name()), class_version<DataType> };
  if (input_file) {
    load_type_ = input_file->type(path);
  }
}

}  // namespace hdtree

// GeneralBranch goes first so that the other
// templates can be partial template specializations of it
#include "hdtree/branch/GeneralBranch.h"
#include "hdtree/branch/AtomicBranch.h"
#include "hdtree/branch/VectorBranch.h"
#include "hdtree/branch/MapBranch.h"

