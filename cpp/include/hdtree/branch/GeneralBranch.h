#pragma once

namespace hdtree {

/**
 * General data set
 *
 * This is the top-level data set that will be used most often.
 * It is meant to be used by a class which registers its member
 * variables to this set via the io::DataSet<DataType>::attach
 * method.
 *
 * More complete documentation is kept in the documentation
 * of the hdtree namespace; nevertheless, a short example
 * is kept here.
 *
 * ```cpp
 * class MyData {
 *  public:
 *   fire_class_version(2);
 *   MyData() = default; // required by serialization technique
 *   // other public members
 *  private:
 *   friend class hdtree::access;
 *   template<typename Branch>
 *   void attach(Branch& b) {
 *     b.attach("my_double",my_double_);
 *     if (b.version() < 2) b.rename("old","new",new_);
 *     else b.attach("new",new_);
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
class Branch : public AbstractBranch<DataType> {
 public:
  /**
   * Flag how a member variable should be accessed by serialization
   *
   * In this context, load is only called when there is a file to load
   * from.
   */
  enum SaveLoad {
    Both,      ///< load and save the member
    LoadOnly,  ///< only load the member (read in)
    SaveOnly   ///< only save the member (write out)
  };

  /**
   * Attach ourselves to the input type after construction.
   *
   * After the intermediate class AbstractBranch does the
   * initialization, we call the `void attach(Branch<DataType>& d)`
   * method of the data pointed to by our handle.
   * This allows us to register its member variables with our own
   * Branch<DataType>::attach method.
   *
   * @param[in] branch_name full in-file branch_name to the data set for this
   * data
   * @param[in] handle address of object already created (optional)
   */
  explicit Branch(const std::string& branch_name, DataType* handle = nullptr)
      : AbstractBranch<DataType>(branch_name, handle) {
    hdtree::access::connect(*this->handle_, *this);
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
   * @throw std::runtime_error if HighFive is unable to load any of the members.
   *
   * @param[in] f file to load from
   */
  void load() final override try {
    for (auto& [save, load, m] : members_)
      if (load) m->load();
  } catch (const HighFive::DataSetException& e) {
    const auto& [memt, memv] = this->save_type_;
    const auto& [diskt, diskv] =
        this->load_type_.value_or(std::make_pair("NULL", -1));
    std::stringstream ss;
    ss << "HDTreeBadType: Data at " << this->name_
       << " could not be loaded into " << memt << " (version " << memv
       << ") from the type it was written as " << diskt << " (version " << diskv
       << ")\n"
          "  Check that your implementation of attach can handle any "
          "previous versions of your class you are trying to read.\n"
          "  H5 Error:\n"
       << e.what();
    throw std::runtime_error(ss.str());
  }

  void attach(Reader& f) final override try {
    this->load_type_ = f.type(this->name_);
    for (auto& [save, load, m] : members_)
      if (load) m->attach(f);
  } catch (const HighFive::DataSetException& e) {
    const auto& [memt, memv] = this->save_type_;
    const auto& [diskt, diskv] = f.type(this->name_);
    std::stringstream ss;
    ss << "HDTreeBadType: Branch " << this->name_
       << " could not be attached to " << memt << " (version " << memv
       << ") from the type it was written as " << diskt << " (version " << diskv
       << ")\n"
          "  Check that your implementation of attach can handle any "
          "previous versions of your class you are trying to read.\n"
          "  H5 Error:\n"
       << e.what();
    throw std::runtime_error(ss.str());
  }

  /*
   * Saving this dataset from the file involves simply saving
   * all of the members of the data type.
   */
  void save() final override {
    for (auto& [save, load, m] : members_)
      if (save) m->save();
  }

  void attach(Writer& f) final override {
    f.structure(this->name_, this->save_type_);
    for (auto& [save, load, m] : members_)
      if (save) m->attach(f);
  }

  /**
   * Attach a member object from the our data handle
   *
   * We create a new child Branch so that we can recursively
   * handle complex member variable types.
   *
   * @tparam MemberType type of member variable we are attaching
   * @param[in] name name of member variable
   * @param[in] m reference of member variable
   * @param[in] save write this member into output files (if the class is being
   * written)
   * @param[in] load load this member from an input file (if being read)
   */
  template <typename MemberType>
  void attach(const std::string& name, MemberType& m,
              SaveLoad sl = SaveLoad::Both) {
    if (name == constants::SIZE_NAME) {
      throw std::runtime_error(
          "HDTreeBadName: The member name '" + constants::SIZE_NAME +
          "' is not allowed due to "
          "its use in the serialization of variable length types.\n"
          "    Please give your member a more detailed name corresponding to "
          "your class");
    }
    bool save{false}, load{false};
    if (sl == SaveLoad::LoadOnly)
      load = true;
    else if (sl == SaveLoad::SaveOnly) {
      save = true;
    } else {
      save = true;
      load = true;
    }
    members_.push_back(std::make_tuple(
        save, load,
        std::make_unique<Branch<MemberType>>(this->name_ + "/" + name, &m)));
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
   * @param[in] new_name name of member variable in version being written to
   * output file
   * @param[in] m reference of member variable
   */
  template <typename MemberType>
  void rename(const std::string& old_name, const std::string& new_name,
              MemberType& m) {
    attach(old_name, m, SaveLoad::LoadOnly);
    attach(new_name, m, SaveLoad::SaveOnly);
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
  std::vector<std::tuple<bool, bool, std::unique_ptr<BaseBranch>>> members_;
  /// pointer to the input file (if there is one)
  Reader* input_file_;
};  // Branch

}  // namespace hdtree
