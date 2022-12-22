#pragma once

#include <boost/core/demangle.hpp>  // for demangling
#include <limits>
#include <optional>
#include <string>

#include "hdtree/ClassVersion.h"
#include "hdtree/Version.h"

namespace hdtree {

class Writer;
class Reader;

/**
 * Empty data base allowing recursion
 *
 * This does not have the type information of the data
 * stored in any of the derived datasets, it simply instructs
 * the derived data sets to define a load and save mechanism
 * for loading/saving the dataset from/to the file.
 *
 * @note Users should never interact with this class.
 */
class BaseBranch {
 public:
  /**
   * define the full name of the branch
   */
  explicit BaseBranch(const std::string& full_name) : name_{full_name} {}

  /**
   * virtual destructor so inherited classes can be properly destructed.
   */
  virtual ~BaseBranch() = default;

  /**
   * pure virtual method for loading data from the input file
   *
   * @param[in] f Reader to load from
   */
  virtual void attach(Reader& f) = 0;

  virtual void load() = 0;

  /**
   * we should persist our hierarchy
   * into the output file
   *
   * @param[in] f Writer to write to
   */
  virtual void attach(Writer& f) = 0;

  /**
   * pure virtual method for saving the current data
   *
   * @param[in] f Writer to write to
   */
  virtual void save() = 0;

  /**
   * pure virtual method for resetting the current data to a blank state
   */
  virtual void clear() = 0;

  /// no copying
  BaseBranch(const BaseBranch&) = delete;
  /// no copying
  BaseBranch& operator=(const BaseBranch&) = delete;

 protected:
  /// name of branch
  std::string name_;
};

/**
 * Type-specific base class to hold common data methods.
 *
 * Most (all I can think of?) have a shared initialization, destruction,
 * getting and setting procedure. We can house these procedures in an
 * intermediary class in the inheritence tree.
 *
 * @tparam DataType type of data being held in this set
 */
template <typename DataType>
class AbstractBranch : public BaseBranch {
 public:
  /**
   * Define the dataset branch_name and provide an optional handle
   *
   * Defines the branch_name to the data set in the file
   * and the handle to the current in-memory version of the data.
   *
   * If the handle is a nullptr, then we will own our own dynamically created
   * copy of the data. If the handle is not a nullptr, then we assume a parent
   * data set is holding the full object and we are simply holding a member
   * variable, so we just copy the address into our handle.
   *
   * This is the location in the code where we require user-defined
   * data classes to be default-constructible.
   *
   * @param[in] name full in-file branch_name to the data set
   * @param[in] handle address of object already created (optional)
   */
  explicit AbstractBranch(const std::string& branch_name,
                          DataType* handle = nullptr)
      : BaseBranch(branch_name), owner_{handle == nullptr} {
    if (owner_) {
      handle_ = new DataType;
    } else {
      handle_ = handle;
    }

    save_type_ = {boost::core::demangle(typeid(DataType).name()),
                  class_version<DataType>};
  }

  /**
   * Delete our object if we own it, otherwise do nothing.
   *
   * @note This is virtual, but I can't think of a good reason to re-implement
   * this function in downstream Branch specializations!
   */
  virtual ~AbstractBranch() {
    if (owner_) delete handle_;
  }

  virtual void attach(Reader& f) = 0;
  /**
   * pure virtual method for loading data
   *
   * @param[in] f Reader to load from
   */
  virtual void load() = 0;

  /**
   * pure virtual method for saving data
   *
   * @param[in] f Writer to save to
   */
  virtual void save() = 0;

  /**
   * pure virtual method for saving structure
   * @param[in] f Writer to write to
   */
  virtual void attach(Writer& f) = 0;

  /**
   * Define the clear function here to handle the most common cases.
   *
   * We 'clear' the object our handle points to.
   * 'clear' means two different things depending on the object.
   * 1. If the object is apart of 'numeric_limits', then we set it to the
   * minimum.
   * 2. Otherwise, we assume the object has the 'clear' method defined.
   *    - This is where we require the user-defined classes to have a
   *      `void clear()` method defined.
   *
   * Case (1) handles the common fundamental types listed in the reference
   * [Numeric Limits](https://en.cppreference.com/w/cpp/types/numeric_limits)
   *
   * Case (2) handles common STL containers as well as std::string and is
   * a simple requirement on user classes.
   *
   * The [`if constexpr`](https://en.cppreference.com/w/cpp/language/if)
   * statement is a C++17 feature that allows this if/else branch to
   * be decided **at compile time**.
   */
  virtual void clear() {
    if (owner_) {
      if constexpr (std::numeric_limits<DataType>::is_specialized) {
        *(this->handle_) = std::numeric_limits<DataType>::min();
      } else {
        handle_->clear();
      }
    }
  }

  /**
   * Get the current in-memory data.
   *
   * @note virtual so that derived data sets
   * could specialize this, but I can't think of a reason to do so.
   *
   * @return const reference to current data
   */
  virtual const DataType& get() const { return *handle_; }

  /**
   * Get the version number for the type we are loading from
   *
   * If we are not loading from anything, we just return the
   * version number of the type we are saving to.
   *
   * @return version number
   */
  int version() const { return load_type_.value_or(save_type_).second; }

  /**
   * Update the in-memory data object with the passed value.
   *
   * @note virtual so that derived data sets could specialize this,
   * but I can't think of a reason to do so.
   *
   * We require that all classes we wish to read/write can
   * use the assignment operator `void operator=(const DataType&)`.
   * This requirement is not reported in the documentation of fire::h5
   * because it is implicitly defined by the compiler unless
   * explicitly deleted by the definer.
   *
   * We perform a deep copy in our handle to the data
   * so that the input object can feel free to go out of scope.
   *
   * @param[in] val new value the in-memory object should be
   */
  virtual void update(const DataType& val) { *handle_ = val; }

  /**
   * Access the in-memory data object
   */
  DataType& operator*() { return *handle_; }

  /**
   * Access the in-memory data opbject in a const manner
   */
  const DataType& operator*() const { return get(); }

  /**
   * Pointer access to the in-memory data type
   */
  DataType* operator->() { return handle_; }

 protected:
  /// type this data is loading from
  std::optional<std::pair<std::string, int>> load_type_;
  /// type this data that is being used to write
  std::pair<std::string, int> save_type_;

  /// handle on current object in memory
  DataType* handle_;
  /// we own the object in memory
  bool owner_;
};  // AbstractBranch

}  // namespace hdtree
