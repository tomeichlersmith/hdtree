#pragma once

namespace hdtree {

/**
 * Branch wrapper for atomic types
 *
 * @see io::is_atomic for how we deduce if a type is atomic
 *
 * Once we finally recurse down to actual fundamental ("atomic") types,
 * we can start actually calling the file load and save methods.
 */
template <typename AtomicType>
class Branch<AtomicType, std::enable_if_t<is_atomic_v<AtomicType>>>
    : public AbstractBranch<AtomicType> {
 public:
  /**
   * We don't do any more initialization except which is handled by the
   * AbstractBranch
   *
   * @param[in] path full in-file path to set holding this data
   * @param[in] handle pointer to already constructed data object (optional)
   */
  explicit Branch(const std::string& path, Reader* input_file = nullptr, AtomicType* handle = nullptr)
      : AbstractBranch<AtomicType>(path, input_file, handle) {}

  /**
   * Down to a type that Reader can handle.
   *
   * @see Reader::load for how we read data from
   * the file at the input path to our handle.
   *
   * @param[in] f Reader to load from
   */
  void load(Reader& f) final override {
    f.load(this->path_, *(this->handle_));
  }

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
};  // Branch<AtomicType>

}
