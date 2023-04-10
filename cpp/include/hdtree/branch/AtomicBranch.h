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
  class ReadBuffer {
    std::size_t max_len_;
    HighFive::DataSet set_;
    std::vector<AtomicType> buffer_;
    std::size_t i_file_;
    std::size_t i_memory_;
    std::size_t entries_;
    /**
     * Load the next chunk of data into memory
     *
     * We determine the size of the next chunk from our own
     * maximum and the number of entries in the data set.
     * We shrink the size of the next chunk depending on how
     * many entries are left if we can't grab a whole maximum
     * sized chunk.
     *
     * We have a compile-time split in order to patch
     * [a bug](https://github.com/BlueBrain/HighFive/issues/490)
     * in HighFive that doesn't allow writing of std::vector<bool>
     * due to the specialization of it **and** to translate
     * our custom enum hdtree::Bool into bools.
     *
     * After reading the next chunk into memory, we update our
     * indicies by resetting the in-memory index to 0 and moving
     * the file index by the size of the buffer.
     *
     * @note We assume that the downstream objects using this buffer
     * know to stop processing before attempting to read passed the
     * end of the data set. We enforce this with an assertion.
     */
    void read_chunk_from_disk() {
      // determine the length we want to request depending
      // on the number of entries left in the file
      std::size_t request_len = this->max_len_;
      if (request_len + i_file_ > entries_) {
        request_len = entries_ - i_file_;
        assert(request_len >= 0);
      }
      // load the next chunk into memory
      if constexpr (std::is_same_v<AtomicType, bool>) {
        /**
         * compile-time split for bools which
         * 1. gets around the std::vector<bool> specialization
         * 2. allows us to translate io::Bool into bools
         */
        std::vector<Bool> buff;
        buff.resize(request_len);
        this->set_.select({i_file_}, {request_len})
            .read(buff.data(), create_enum_bool());
        buffer_.reserve(buff.size());
        for (const auto& v : buff) buffer_.push_back(v == Bool::TRUE);
      } else {
        this->set_.select({i_file_}, {request_len}).read(buffer_);
      }
      // update indices
      i_file_ += buffer_.size();
      i_memory_ = 0;
    }

   public:
    explicit ReadBuffer(std::size_t max, HighFive::DataSet s)
        : max_len_{max}, set_{s}, buffer_{}, i_file_{0}, i_memory_{0} {
      entries_ = this->set_.getDimensions().at(0);
      this->read_chunk_from_disk();
      // TODO deduce max_len from chunk size of dataset
    }
    void read(AtomicType& v) {
      if (i_memory_ == buffer_.size()) this->read_chunk_from_disk();
      v = buffer_[i_memory_];
      ++i_memory_;
    }
  };
  std::unique_ptr<ReadBuffer> read_buffer_;

  class WriteBuffer {
    std::size_t max_len_;
    HighFive::DataSet set_;
    std::vector<AtomicType> buffer_;
    std::size_t i_file_;
    /**
     * Flush our in-memory buffer onto disk
     *
     * We leave early if the buffer is empty.
     * This is helpful for the case where the number of elements
     * in the dataset happen to be an exact multiple of the buffer
     * size. Then the buffer would be empty at the time that
     * Writer::~Writer is called which calls all Buffers to flush
     * in order to avoid data loss.
     *
     * We determine the new extent of the dataset given how many
     * elements are in the buffer, then we resize the dataset
     * that is on disk to this new extent.
     *
     * Then, we copy the buffer into the DataSet on disk using
     * a compile-time choice that handles the std::vector<bool>
     * specialization
     * [bug in HighFive](https://github.com/BlueBrain/HighFive/issues/490).
     * *and* translates bools into our custom enum hdtree::Bool
     * which mimics the serialization behavior of the bool type
     * understandable by h5py.
     *
     * Finally, we update the file index, clear the buffer,
     * and re-reserve the maximum length of the buffer to prepare
     * for another chunk of data.
     *
     * @throws HighFive::DataSetException if unable to extend or
     * write to the DataSet.
     */
    void flush() {
      if (buffer_.size() == 0) return;
      std::size_t new_extent = i_file_ + buffer_.size();
      // throws if not created yet
      if (this->set_.getDimensions().at(0) < new_extent) {
        this->set_.resize({new_extent});
      }
      if constexpr (std::is_same_v<AtomicType, bool>) {
        // handle bool specialization
        std::vector<Bool> buff;
        buff.reserve(buffer_.size());
        for (const auto& v : buffer_)
          buff.push_back(v ? Bool::TRUE : Bool::FALSE);
        this->set_.select({i_file_}, {buffer_.size()}).write(buff);
      } else {
        this->set_.select({i_file_}, {buffer_.size()}).write(buffer_);
      }
      i_file_ += buffer_.size();
      buffer_.clear();
      buffer_.reserve(this->max_len_);
    }

   public:
    /**
     * Define the buffer size and the set we will write to
     *
     * We also use std::vector::reserve to let the memory
     * handler know the size of our buffer. This can help
     * us avoid unnecessary copying and reallocation while
     * using std::vector::push_back to insert elements into
     * the vector.
     *
     * @param[in] max buffer size
     * @param[in] s dataset to write to
     */
    explicit WriteBuffer(std::size_t max, HighFive::DataSet s)
        : max_len_{max}, set_{s}, buffer_{}, i_file_{0} {
      buffer_.reserve(this->max_len_);
    }

    // flush before deleting
    ~WriteBuffer() { flush(); }

    /**
     * Put the new value into the buffer
     *
     * If the buffer goes over the maximum length of the buffer,
     * then we call Buffer::flush
     *
     * @param[in] val data to append to the dataset
     */
    void save(const AtomicType& val) {
      buffer_.push_back(val);
      if (buffer_.size() > this->max_len_) flush();
    }
  };

  std::unique_ptr<WriteBuffer> write_buffer_;

 public:
  /**
   * We don't do any more initialization except which is handled by the
   * AbstractBranch
   *
   * @param[in] branch_name full in-file branch_name to set holding this data
   * @param[in] handle pointer to already constructed data object (optional)
   */
  explicit Branch(const std::string& branch_name, AtomicType* handle = nullptr)
      : AbstractBranch<AtomicType>(branch_name, handle) {}

  void attach(Reader& f) final override try {
    // deletes old read_buffer_ if there was one already constructed
    read_buffer_ =
        std::make_unique<ReadBuffer>(10000, f.getDataSet(this->name_));
  } catch (const HighFive::DataSetException& e) {
    // an exception was thrown when we tried to `get` the dataset by name
    std::stringstream msg, help;
    msg << "HDTreeBadType: Branch at " << this->name_
        << " could not be accessed.";
    help << "Check that this branch exists in your HDTree.\n"
            "    H5 Error: " << e.what();
    throw HDTreeException(msg.str(), help.str());
  }

  /**
   * Down to a type that Reader can handle.
   *
   * @see Reader::load for how we read data from
   * the file at the input branch_name to our handle.
   *
   * @param[in] f Reader to load from
   */
  void load() final override {
    if (read_buffer_) read_buffer_->read(*(this->handle_));
  }

  /**
   * Down to a type that io::Writer can handle
   *
   * @see io::Writer::save for how we write data to
   * the file at the input branch_name from our handle.
   *
   * @param[in] f io::Writer to save to
   */
  void save() final override {
    if (write_buffer_) write_buffer_->save(*(this->handle_));
  }

  /**
   * do NOT persist any structure for atomic types
   *
   * The atomic types are translated into H5 DataSets in Writer::save
   * where the types are persisted as well.
   */
  void attach(Writer& f) final override try {
    HighFive::DataType t;
    if constexpr (std::is_same_v<AtomicType, bool>) {
      t = create_enum_bool();
    } else {
      t = HighFive::AtomicType<AtomicType>();
    }
    auto ds = f.createDataSet(this->name_, t);
    ds.createAttribute(constants::TYPE_ATTR_NAME,
                       boost::core::demangle(typeid(AtomicType).name()));
    ds.createAttribute(constants::VERS_ATTR_NAME, 0);
    // flush and deletes old buffer if it exists
    write_buffer_ = std::make_unique<WriteBuffer>(f.getRowsPerChunk(), ds);
  } catch (const HighFive::DataSetException& e) {
    // an exception was thrown when we tried to create the dataset by name
    std::stringstream msg, help;
    msg << "HDTreeBadType: Branch at " << this->name_
        << " could not be created.";
    help << "Check that this branch does not already exist in your HDTree.\n"
            "      H5 Error:  " << e.what();
    throw HDTreeException(msg.str(), help.str());
  }
};  // Branch<AtomicType>

}  // namespace hdtree
