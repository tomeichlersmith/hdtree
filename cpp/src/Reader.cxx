#include "hdtree/Reader.h"

#include "hdtree/Branch.h"
#include "hdtree/Constants.h"

namespace hdtree {

Reader::Reader(const std::pair<std::string, std::string>& file_tree_path,
               bool inplace)
    : file_{file_tree_path.first,
            inplace ? HighFive::File::ReadWrite : HighFive::File::ReadOnly},
      tree_{file_.getGroup(file_tree_path.second)} {
  HighFive::Attribute size_attr = tree_.getAttribute(constants::SIZE_NAME);
  size_attr.read(entries_);
}

std::string Reader::name() const { return file_.getName(); }

std::vector<std::string> Reader::list(const std::string& group_path) const {
  // just return empty list of group does not exist
  if (not tree_.exist(group_path)) return {};
  return tree_.getGroup(group_path).listObjectNames();
}

HighFive::DataSet Reader::getDataSet(const std::string& branch_name) const {
  return tree_.getDataSet(branch_name);
}

HighFive::DataType Reader::getDataSetType(const std::string& dataset) const {
  return getDataSet(dataset).getDataType();
}

HighFive::ObjectType Reader::getH5ObjectType(const std::string& path) const {
  return tree_.getObjectType(path);
}

std::vector<std::pair<std::string, std::string>> Reader::availableObjects() {
  // TODO recursively list branches in tree
  return {};
}

std::pair<std::string, int> Reader::type(const std::string& branch_name) {
  HighFive::Attribute type_attr =
      getH5ObjectType(branch_name) == HighFive::ObjectType::Dataset
          ? tree_.getDataSet(branch_name)
                .getAttribute(constants::TYPE_ATTR_NAME)
          : tree_.getGroup(branch_name).getAttribute(constants::TYPE_ATTR_NAME);
  HighFive::Attribute vers_attr =
      getH5ObjectType(branch_name) == HighFive::ObjectType::Dataset
          ? tree_.getDataSet(branch_name)
                .getAttribute(constants::VERS_ATTR_NAME)
          : tree_.getGroup(branch_name).getAttribute(constants::VERS_ATTR_NAME);

  std::string type;
  type_attr.read(type);

  int vers;
  vers_attr.read(vers);
  return std::make_pair(type, vers);
}

void Reader::mirror(const std::string& branch_name, Writer& output) {
  // only mirror structure of groups
  if (getH5ObjectType(branch_name) != HighFive::ObjectType::Group) return;
  // copy over type attributes creating the group in the output file
  output.structure(branch_name, this->type(branch_name));
  // recurse into subobjects
  for (auto& subgrp : this->list(branch_name))
    mirror(branch_name + "/" + subgrp, output);
}

void Reader::copy(unsigned int long i_entry, const std::string& branch_name,
                  Writer& output) {
  if (mirror_objects_.find(branch_name) == mirror_objects_.end()) {
    /**
     * this is where recursing into the subgroups of full_name occurs
     * if this mirror object hasn't been created yet
     *
     * If a mirror object is being created that means the event object
     * satisfies the following:
     * 1. The event object exists in the input file
     * 2. The event object should be written to the output file
     * 3. The event object HAS NOT been accessed during the processing
     *    of this input file yet
     *
     * This means we should copy over the structure of this event object
     * from the input file into the output file.
     */
    mirror(branch_name, output);
    mirror_objects_.emplace(std::make_pair(
        branch_name,
        std::make_unique<MirrorObject>(branch_name, *this, output)));
  }
  // do the copying
  mirror_objects_[branch_name]->copy(i_entry, 1);
}

Reader::MirrorObject::MirrorObject(const std::string& branch_name,
                                   Reader& reader, Writer& writer) {
  if (reader.getH5ObjectType(branch_name) == HighFive::ObjectType::Dataset) {
    // simple atomic event object
    //  unfortunately, I can't think of a better solution than manually
    //  copying the code for all of the types
    HighFive::DataType type = reader.getDataSetType(branch_name);
    if (type == HighFive::create_datatype<int>()) {
      data_ = std::make_unique<Branch<int>>(branch_name);
    } else if (type == HighFive::create_datatype<long int>()) {
      data_ = std::make_unique<Branch<long int>>(branch_name);
    } else if (type == HighFive::create_datatype<long long int>()) {
      data_ = std::make_unique<Branch<long long int>>(branch_name);
    } else if (type == HighFive::create_datatype<unsigned int>()) {
      data_ = std::make_unique<Branch<unsigned int>>(branch_name);
    } else if (type == HighFive::create_datatype<unsigned long int>()) {
      data_ = std::make_unique<Branch<unsigned long int>>(branch_name);
    } else if (type == HighFive::create_datatype<unsigned long long int>()) {
      data_ = std::make_unique<Branch<unsigned long long int>>(branch_name);
    } else if (type == HighFive::create_datatype<float>()) {
      data_ = std::make_unique<Branch<float>>(branch_name);
    } else if (type == HighFive::create_datatype<double>()) {
      data_ = std::make_unique<Branch<double>>(branch_name);
    } else if (type == HighFive::create_datatype<std::string>()) {
      data_ = std::make_unique<Branch<std::string>>(branch_name);
    } else if (type == HighFive::create_datatype<hdtree::Bool>()) {
      data_ = std::make_unique<Branch<bool>>(branch_name);
    } else {
      throw HDTreeException(
          "HDTreeUnknownDS: Unable to deduce C++ type "
          "from H5 type during a copy",
          "This is a very deep exception and can come from many "
          "different places, but most commonly this comes from "
          "writing a custom HDF5 Data Type into a file. Since "
          "HDTree does not know how to load these custom data "
          "type descriptions, it is unable to load this type of "
          "data in a branch for copying.");
    }
    data_->attach(reader);
    data_->attach(writer);
  } else {
    // event object is a H5 group meaning it is more complicated
    // than a simple atomic type
    auto subobjs = reader.list(branch_name);
    for (auto& subobj : subobjs) {
      std::string sub_path{branch_name + "/" + subobj};
      if (subobj == constants::SIZE_NAME) {
        size_member_ = std::make_unique<Branch<std::size_t>>(sub_path);
        size_member_->attach(reader);
        size_member_->attach(writer);
      } else {
        obj_members_.emplace_back(
            std::make_unique<MirrorObject>(sub_path, reader, writer));
      }
    }
  }
}

void Reader::MirrorObject::copy(unsigned long int i_entry,
                                unsigned long int n) {
  unsigned long int num_to_advance{
      i_entry <= last_entry_ ? 0 : i_entry - last_entry_ - 1},
      num_to_save{n};
  last_entry_ = i_entry;

  // if we have a data member, the data member is the only part of this
  // mirror object
  if (data_) {
    // load until one before desired entry
    for (std::size_t i{0}; i < num_to_advance; i++) data_->load();
    // load and save desired entries
    for (std::size_t i{0}; i < num_to_save; i++) {
      data_->load();
      data_->save();
    }
    return;
  }

  /// if there is a member determining the size of each entry,
  /// we need to follow its lead
  if (size_member_) {
    unsigned long int new_num_to_advance{0};
    for (std::size_t i{0}; i < num_to_advance; i++) {
      size_member_->load();
      new_num_to_advance +=
          dynamic_cast<Branch<std::size_t>&>(*size_member_).get();
    }
    unsigned long int new_num_to_save = 0;
    for (std::size_t i{0}; i < num_to_save; i++) {
      size_member_->load();
      new_num_to_save +=
          dynamic_cast<Branch<std::size_t>&>(*size_member_).get();
      size_member_->save();
    }

    num_to_advance = new_num_to_advance;
    num_to_save = new_num_to_save;
  }

  for (auto& obj : obj_members_) obj->copy(num_to_advance, num_to_save);
}

}  // namespace hdtree
