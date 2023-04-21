#pragma once

namespace hdtree {

/**
 * Our wrapper around std::vector
 *
 * @note We assume that the load/save is done sequentially.
 * This assumption is made because
 *  (1) it is how HDTree is designed and
 *  (2) it allows us to not have to store
 *      as much metadata about the vectors.
 *
 * @tparam ContentType type of object stored within the std::vector
 */
template <typename ContentType>
class Branch<std::vector<ContentType>>
    : public AbstractBranch<std::vector<ContentType>> {
  hdtree_class_version(1);

 public:
  /**
   * We create two child data sets, one to hold the successive sizes of the
   * vectors and one to hold all of the data in all of the vectors serially.
   *
   * @param[in] branch_name full in-file branch_name to set holding this data
   * @param[in] handle pointer to object already constructed (optional)
   */
  explicit Branch(const std::string& branch_name,
                  std::vector<ContentType>* handle = nullptr)
      : AbstractBranch<std::vector<ContentType>>(branch_name, handle),
        size_{branch_name + "/" + constants::SIZE_NAME},
        data_{branch_name + "/data"} {}

  void attach(Reader& f) final override {
    this->load_type_ = f.type(this->name_);
    size_.attach(f);
    data_.attach(f);
  }
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
  void load() final override {
    size_.load();
    this->handle_->resize(size_.get());
    for (std::size_t i_vec{0}; i_vec < size_.get(); i_vec++) {
      data_.load();
      (*(this->handle_))[i_vec] = data_.get();
    }
  }

  /**
   * Save a vector to the output file
   *
   * @note We assume that the saves are done sequentially.
   *
   * We write the size and the content onto the end of their data sets.
   *
   * @param[in] f io::Writer to save to
   */
  void save() final override {
    size_.update(this->handle_->size());
    size_.save();
    for (std::size_t i_vec{0}; i_vec < this->handle_->size(); i_vec++) {
      data_.update(this->handle_->at(i_vec));
      data_.save();
    }
  }

  void attach(Writer& f) final override {
    f.structure(this->name_, this->save_type_);
    size_.attach(f);
    data_.attach(f);
  }

 private:
  /// the data set of sizes of the vectors
  Branch<std::size_t> size_;
  /// the data set holding the content of all the vectors
  Branch<ContentType> data_;
};  // Branch<std::vector>

}  // namespace hdtree
