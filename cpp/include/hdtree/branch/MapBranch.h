#pragma once

namespace hdtree {

/**
 * Our wrapper around std::map
 *
 * Very similar implementation as vectors, just having
 * two columns rather than only one.
 *
 * @note We assume the load/save is done sequentially.
 * Similar rational as io::Branch<std::vector<ContentType>>
 *
 * @tparam KeyType type that the keys in the map are
 * @tparam ValType type that the vals in the map are
 */
template <typename KeyType, typename ValType>
class Branch<std::map<KeyType,ValType>>
    : public AbstractBranch<std::map<KeyType,ValType>> {
  fire_class_version(1);
 public:
  /**
   * We create three child data sets, one for the successive sizes
   * of the maps and two to hold all the keys and values serially.
   *
   * @param[in] path full in-file path to set holding this data
   * @param[in] handle pointer to object already constructed (optional)
   */
  explicit Branch(const std::string& path, Reader* input_file = nullptr, 
      std::map<KeyType,ValType>* handle = nullptr)
      : AbstractBranch<std::map<KeyType,ValType>>(path, input_file, handle),
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
  void load(Reader& f) final override {
    size_.load(f);
    for (std::size_t i_map{0}; i_map < size_.get(); i_map++) {
      keys_.load(f);
      vals_.load(f);
      this->handle_->emplace(keys_.get(), vals_.get());
    }
  }

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
  Branch<std::size_t> size_;
  /// the data set holding the content of all the keys
  Branch<KeyType> keys_;
  /// the data set holding the content of all the vals
  Branch<ValType> vals_;
};  // Branch<std::map>

}
