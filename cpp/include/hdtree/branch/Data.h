#ifndef FIRE_IO_DATA_H
#define FIRE_IO_DATA_H

#include <memory>
#include <type_traits>
#include <vector>
#include <map>

#include "fire/exception/Exception.h"
#include "fire/io/Access.h"
#include "fire/io/AbstractData.h"
#include "fire/io/Writer.h"
#include "fire/io/Constants.h"
#include "fire/io/h5/Reader.h"
#ifdef fire_USE_ROOT
#include "fire/io/root/Reader.h"
#endif

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
 * Below is a sketch of how the various fire::io::Data template classes
 * interact with each other and the broader fire ecosystem.
 *
 * @image html fire_io_Data_AccessPattern.svg
 */
namespace fire::io {

// need implementation here so we can use the fully defined input file `type` function
template <typename DataType>
AbstractData<DataType>::AbstractData(const std::string& path, Reader* input_file, DataType* handle)
  : BaseData(path), owner_{handle == nullptr} {
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

/**
 * General data set
 *
 * This is the top-level data set that will be used most often.
 * It is meant to be used by a class which registers its member
 * variables to this set via the io::DataSet<DataType>::attach
 * method.
 *
 * More complete documentation is kept in the documentation
 * of the fire::io namespace; nevertheless, a short example
 * is kept here.
 *
 * ```cpp
 * class MyData {
 *  public:
 *   fire_class_version(2);
 *   MyData() = default; // required by serialization technique
 *   // other public members
 *  private:
 *   friend class fire::io::access;
 *   template<typename Data>
 *   void attach(Data& d) {
 *     d.attach("my_double",my_double_);
 *     if (d.version() < 2) d.rename("old","new",new_);
 *     else d.attach("new",new_);
 *   }
 *   void clear() {
 *     my_double_ = -1; // reset to default value
 *     new_ = 0;
 *   }
 *  private:
 *   double my_double_;
 *   int new_;
 *   // this member doesn't appear in 'attach' so it won't end up on disk
 *   int i_wont_be_on_disk_;
 * };
 * ```
 */
template <typename DataType, typename Enable = void>
class Data : public AbstractData<DataType> {
 public:
  /**
   * Flag how a member variable should be accessed by serialization
   *
   * In this context, load is only called when there is a file to load
   * from.
   */
  enum SaveLoad {
    Both, ///< load and save the member
    LoadOnly, ///< only load the member (read in)
    SaveOnly  ///< only save the member (write out)
  };

  /**
   * Attach ourselves to the input type after construction.
   *
   * After the intermediate class AbstractData does the
   * initialization, we call the `void attach(io::Data<DataType>& d)`
   * method of the data pointed to by our handle. 
   * This allows us to register its member variables with our own 
   * io::Data<DataType>::attach method.
   *
   * @param[in] path full in-file path to the data set for this data
   * @param[in] handle address of object already created (optional)
   */
  explicit Data(const std::string& path, Reader* input_file = nullptr, DataType* handle = nullptr)
      : AbstractData<DataType>(path, input_file, handle), input_file_{input_file} {
    fire::io::access::connect(*this->handle_, *this);
  }

  /**
   * Loading this dataset from the file involves simply loading
   * all of the members of the data type.
   *
   * We catch a HighFive exception early here so that we can
   * detail to the user which class is causing the read issue.
   * This is especially helpful in the case of containers of user
   * types since the issue is (probably) not coming from serialization
   * of the container.
   *
   * @throw Exception if HighFive is unable to load any of the members.
   *
   * @param[in] f file to load from
   */
  void load(h5::Reader& f) final override try {
    for (auto& [save,load,m] : members_) if (load) m->load(f);
  } catch (const HighFive::DataSetException& e) {
    const auto& [memt, memv] = this->save_type_;
    const auto& [diskt, diskv] = f.type(this->path_);
    std::stringstream ss;
    ss << "Data at " << this->path_ << " could not be loaded into "
        << memt  << " (version " << memv << ") from the type it was written as " 
        << diskt << " (version " << diskv << ")\n"
        "  Check that your implementation of attach can handle any "
        "previous versions of your class you are trying to read.\n"
        "  H5 Error:\n" << e.what();
    throw Exception("BadType",ss.str(), false);
  }

#ifdef fire_USE_ROOT
  /**
   * Loading this dataset from a ROOT file involves giving
   * it directly to the file immediately.
   */
  void load(root::Reader& f) final override {
    f.load(this->path_, *(this->handle_));
  }
#endif

  /*
   * Saving this dataset from the file involves simply saving
   * all of the members of the data type.
   *
   * @param[in] f file to save to
   */
  void save(Writer& f) final override {
    for (auto& [save,load,m] : members_) if (save) m->save(f);
  }

  void structure(Writer& f) final override {
    f.structure(this->path_, this->save_type_);
    for (auto& [save,load,m] : members_) if (save) m->structure(f);
  }

  /**
   * Attach a member object from the our data handle
   *
   * We create a new child Data so that we can recursively
   * handle complex member variable types.
   *
   * @tparam MemberType type of member variable we are attaching
   * @param[in] name name of member variable
   * @param[in] m reference of member variable
   * @param[in] save write this member into output files (if the class is being written)
   * @param[in] load load this member from an input file (if being read)
   */
  template <typename MemberType>
  void attach(const std::string& name, MemberType& m, SaveLoad sl = SaveLoad::Both) {
    if (name == constants::SIZE_NAME) {
      throw Exception("BadName",
          "The member name '"+constants::SIZE_NAME+"' is not allowed due to "
          "its use in the serialization of variable length types.\n"
          "    Please give your member a more detailed name corresponding to "
          "your class", false);
    }
    bool save{false}, load{false};
    Reader* input_file{input_file_};
    if (sl == SaveLoad::LoadOnly) load = true;
    else if (sl == SaveLoad::SaveOnly) { save = true; input_file = nullptr; }
    else { save = true; load = true; }
    members_.push_back(std::make_tuple(save, load,
        std::make_unique<Data<MemberType>>(this->path_ + "/" + name, input_file, &m)));
  }

  /**
   * Rename a member variable
   *
   * This is a simple helper-function wrapping attach which does the two
   * calls for the user sharing the same member variable, creating one
   * member for loading from the old_name and one member for writing
   * to the new_name.
   *
   * @tparam MemberType type of member variable we are attaching
   * @param[in] old_name name of member variable in version being read from file
   * @param[in] new_name name of member variable in version being written to output file
   * @param[in] m reference of member variable
   */
  template <typename MemberType>
  void rename(const std::string& old_name, const std::string& new_name, MemberType& m) {
    attach(old_name,m,SaveLoad::LoadOnly);
    attach(new_name,m,SaveLoad::SaveOnly);
  }

 private:
  /**
   * list of members in this dataset
   *
   * the extra boolean flags are to tell us if that member 
   * should be loaded from the input file and/or saved
   * to the output file
   *
   * This is the core of schema evolution.
   */
  std::vector<std::tuple<bool,bool,std::unique_ptr<BaseData>>> members_;
  /// pointer to the input file (if there is one)
  Reader* input_file_;
};  // Data

/**
 * Data wrapper for atomic types
 *
 * @see io::is_atomic for how we deduce if a type is atomic
 *
 * Once we finally recurse down to actual fundamental ("atomic") types,
 * we can start actually calling the file load and save methods.
 */
template <typename AtomicType>
class Data<AtomicType, std::enable_if_t<is_atomic_v<AtomicType>>>
    : public AbstractData<AtomicType> {
 public:
  /**
   * We don't do any more initialization except which is handled by the
   * AbstractData
   *
   * @param[in] path full in-file path to set holding this data
   * @param[in] handle pointer to already constructed data object (optional)
   */
  explicit Data(const std::string& path, Reader* input_file = nullptr, AtomicType* handle = nullptr)
      : AbstractData<AtomicType>(path, input_file, handle) {}

  /**
   * Down to a type that h5::Reader can handle.
   *
   * @see h5::Reader::load for how we read data from
   * the file at the input path to our handle.
   *
   * @param[in] f h5::Reader to load from
   */
  void load(h5::Reader& f) final override {
    f.load(this->path_, *(this->handle_));
  }

#ifdef fire_USE_ROOT
  /**
   * Loading this dataset from a ROOT file involves giving
   * it directly to the file immediately.
   */
  void load(root::Reader& f) final override {
    f.load(this->path_, *(this->handle_));
  }
#endif

  /**
   * Down to a type that io::Writer can handle
   *
   * @see io::Writer::save for how we write data to
   * the file at the input path from our handle.
   *
   * @param[in] f io::Writer to save to
   */
  void save(Writer& f) final override {
    f.save(this->path_, *(this->handle_));
  }

  /**
   * do NOT persist any structure for atomic types
   *
   * The atomic types are translated into H5 DataSets in Writer::save
   * where the types are persisted as well.
   */
  void structure(Writer& f) final override {
    // atomic types get translated into H5 DataSets
    // in save so we purposefully DO NOTHING here
  }
};  // Data<AtomicType>

/**
 * Our wrapper around std::vector
 *
 * @note We assume that the load/save is done sequentially.
 * This assumption is made because
 *  (1) it is how fire is designed and
 *  (2) it allows us to not have to store
 *      as much metadata about the vectors.
 *
 * @tparam ContentType type of object stored within the std::vector
 */
template <typename ContentType>
class Data<std::vector<ContentType>>
    : public AbstractData<std::vector<ContentType>> {
  fire_class_version(1);
 public:
  /**
   * We create two child data sets, one to hold the successive sizes of the
   * vectors and one to hold all of the data in all of the vectors serially.
   *
   * @param[in] path full in-file path to set holding this data
   * @param[in] handle pointer to object already constructed (optional)
   */
  explicit Data(const std::string& path, Reader* input_file = nullptr, 
      std::vector<ContentType>* handle = nullptr)
      : AbstractData<std::vector<ContentType>>(path, input_file, handle),
        size_{path + "/" + constants::SIZE_NAME, input_file},
        data_{path + "/data", input_file} {}

  /**
   * Load a vector from the input file
   *
   * @note We assume that the loads are done sequentially.
   *
   * We read the next size and then read that many items from
   * the content data set into the vector handle.
   *
   * @param[in] f h5::Reader to load from
   */
  void load(h5::Reader& f) final override {
    size_.load(f);
    this->handle_->resize(size_.get());
    for (std::size_t i_vec{0}; i_vec < size_.get(); i_vec++) {
      data_.load(f);
      (*(this->handle_))[i_vec] = data_.get();
    }
  }

#ifdef fire_USE_ROOT
  /**
   * Loading this dataset from a ROOT file involves giving
   * it directly to the file immediately.
   */
  void load(root::Reader& f) final override {
    f.load(this->path_, *(this->handle_));
  }
#endif

  /**
   * Save a vector to the output file
   *
   * @note We assume that the saves are done sequentially.
   *
   * We write the size and the content onto the end of their data sets.
   *
   * @param[in] f io::Writer to save to
   */
  void save(Writer& f) final override {
    size_.update(this->handle_->size());
    size_.save(f);
    for (std::size_t i_vec{0}; i_vec < this->handle_->size(); i_vec++) {
      data_.update(this->handle_->at(i_vec));
      data_.save(f);
    }
  }

  void structure(Writer& f) final override {
    f.structure(this->path_, this->save_type_);
    size_.structure(f);
    data_.structure(f);
  }

 private:
  /// the data set of sizes of the vectors
  Data<std::size_t> size_;
  /// the data set holding the content of all the vectors
  Data<ContentType> data_;
};  // Data<std::vector>

/**
 * Our wrapper around std::map
 *
 * Very similar implementation as vectors, just having
 * two columns rather than only one.
 *
 * @note We assume the load/save is done sequentially.
 * Similar rational as io::Data<std::vector<ContentType>>
 *
 * @tparam KeyType type that the keys in the map are
 * @tparam ValType type that the vals in the map are
 */
template <typename KeyType, typename ValType>
class Data<std::map<KeyType,ValType>>
    : public AbstractData<std::map<KeyType,ValType>> {
  fire_class_version(1);
 public:
  /**
   * We create three child data sets, one for the successive sizes
   * of the maps and two to hold all the keys and values serially.
   *
   * @param[in] path full in-file path to set holding this data
   * @param[in] handle pointer to object already constructed (optional)
   */
  explicit Data(const std::string& path, Reader* input_file = nullptr, 
      std::map<KeyType,ValType>* handle = nullptr)
      : AbstractData<std::map<KeyType,ValType>>(path, input_file, handle),
        size_{path + "/" + constants::SIZE_NAME, input_file},
        keys_{path + "/keys", input_file},
        vals_{path + "/vals", input_file} {}

  /**
   * Load a map from the input file
   *
   * @note We assume that the loads are done sequentially.
   *
   * We read the next size and then read that many items from
   * the keys/vals data sets into the map handle.
   *
   * @param[in] f h5::Reader to load from
   */
  void load(h5::Reader& f) final override {
    size_.load(f);
    for (std::size_t i_map{0}; i_map < size_.get(); i_map++) {
      keys_.load(f);
      vals_.load(f);
      this->handle_->emplace(keys_.get(), vals_.get());
    }
  }

#ifdef fire_USE_ROOT
  /**
   * Loading this dataset from a ROOT file involves giving
   * it directly to the file immediately.
   */
  void load(root::Reader& f) final override {
    f.load(this->path_, *(this->handle_));
  }
#endif

  /**
   * Save a vector to the output file
   *
   * @note We assume that the saves are done sequentially.
   *
   * We write the size and the keys/vals onto the end of their data sets.
   *
   * @param[in] f io::Writer to save to
   */
  void save(Writer& f) final override {
    size_.update(this->handle_->size());
    size_.save(f);
    for (auto const& [key,val] : *(this->handle_)) {
      keys_.update(key);
      keys_.save(f);
      vals_.update(val);
      vals_.save(f);
    }
  }

  void structure(Writer& f) final override {
    f.structure(this->path_, this->save_type_);
    size_.structure(f);
    keys_.structure(f);
    vals_.structure(f);
  }

 private:
  /// the data set of sizes of the vectors
  Data<std::size_t> size_;
  /// the data set holding the content of all the keys
  Data<KeyType> keys_;
  /// the data set holding the content of all the vals
  Data<ValType> vals_;
};  // Data<std::map>

}  // namespace fire::io
  
#endif  // FIRE_H5_DATASET_H

