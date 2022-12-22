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
class Branch<std::map<KeyType, ValType>>
    : public AbstractBranch<std::map<KeyType, ValType>> {
  fire_class_version(1);

 public:
  /**
   * We create three child data sets, one for the successive sizes
   * of the maps and two to hold all the keys and values serially.
   *
   * @param[in] branch_name full in-file branch_name to set holding this data
   * @param[in] handle pointer to object already constructed (optional)
   */
  explicit Branch(const std::string& branch_name,
                  std::map<KeyType, ValType>* handle = nullptr)
      : AbstractBranch<std::map<KeyType, ValType>>(branch_name, handle),
        size_{branch_name + "/" + constants::SIZE_NAME},
        keys_{branch_name + "/keys"},
        vals_{branch_name + "/vals"} {}

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
  void load() final override {
    size_.load();
    for (std::size_t i_map{0}; i_map < size_.get(); i_map++) {
      keys_.load();
      vals_.load();
      this->handle_->emplace(keys_.get(), vals_.get());
    }
  }

  void attach(Reader& f) final override {
    this->load_type_ = f.type(this->name_);
    size_.attach(f);
    keys_.attach(f);
    vals_.attach(f);
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
  void save() final override {
    size_.update(this->handle_->size());
    size_.save();
    for (auto const& [key, val] : *(this->handle_)) {
      keys_.update(key);
      keys_.save();
      vals_.update(val);
      vals_.save();
    }
  }

  void attach(Writer& f) final override {
    f.structure(this->name_, this->save_type_);
    size_.attach(f);
    keys_.attach(f);
    vals_.attach(f);
  }

 private:
  /// the data set of sizes of the vectors
  Branch<std::size_t> size_;
  /// the data set holding the content of all the keys
  Branch<KeyType> keys_;
  /// the data set holding the content of all the vals
  Branch<ValType> vals_;
};  // Branch<std::map>

}  // namespace hdtree
