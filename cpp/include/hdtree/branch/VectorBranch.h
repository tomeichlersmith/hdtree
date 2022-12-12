#pragma once

namespace hdtree {

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
class Branch<std::vector<ContentType>>
    : public AbstractBranch<std::vector<ContentType>> {
  fire_class_version(1);
 public:
  /**
   * We create two child data sets, one to hold the successive sizes of the
   * vectors and one to hold all of the data in all of the vectors serially.
   *
   * @param[in] path full in-file path to set holding this data
   * @param[in] handle pointer to object already constructed (optional)
   */
  explicit Branch(const std::string& path, Reader* input_file = nullptr, 
      std::vector<ContentType>* handle = nullptr)
      : AbstractBranch<std::vector<ContentType>>(path, input_file, handle),
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
  void load(Reader& f) final override {
    size_.load(f);
    this->handle_->resize(size_.get());
    for (std::size_t i_vec{0}; i_vec < size_.get(); i_vec++) {
      data_.load(f);
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
  Branch<std::size_t> size_;
  /// the data set holding the content of all the vectors
  Branch<ContentType> data_;
};  // Branch<std::vector>

}
