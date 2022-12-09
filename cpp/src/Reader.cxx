#include "hdtree/Reader.h"

#include "hdtree/Constants.h"
#include "hdtree/Branch.h"

namespace hdtree {

Reader::Reader(const std::string& file_path, const std::string& tree_path) 
  : file_{file_path}, tree_{file_.getGroup(tree_path)} {
  HighFive::Attribute size_attr = tree_.getAttribute(constants::SIZE_NAME);
  size_attr.read(entries_);
}

void Reader::load_into(BaseBranch& d) {
  d.load(*this);
}

std::string Reader::name() const { return file_.getName(); }

std::vector<std::string> Reader::list(const std::string& group_path) const {
  // just return empty list of group does not exist
  if (not tree_.exist(group_path)) return {};
  return tree_.getGroup(group_path).listObjectNames();
}

HighFive::DataType Reader::getDataSetType(const std::string& dataset) const {
  return tree_.getDataSet(dataset).getDataType();
}

HighFive::ObjectType Reader::getH5ObjectType(const std::string& path) const {
  return tree_.getObjectType(path);
}

std::vector<std::pair<std::string,std::string>> Reader::availableObjects() {
  // TODO recursively list branches in tree
  return {};
}

std::pair<std::string,int> Reader::type(const std::string& path) {
  HighFive::Attribute type_attr = 
    getH5ObjectType(path) == HighFive::ObjectType::Dataset
          ? tree_.getDataSet(path).getAttribute(constants::TYPE_ATTR_NAME)
          : tree_.getGroup(path).getAttribute(constants::TYPE_ATTR_NAME);
  HighFive::Attribute vers_attr = 
    getH5ObjectType(path) == HighFive::ObjectType::Dataset
          ? tree_.getDataSet(path).getAttribute(constants::VERS_ATTR_NAME)
          : tree_.getGroup(path).getAttribute(constants::VERS_ATTR_NAME);

  std::string type;
  type_attr.read(type);

  int vers;
  vers_attr.read(vers);
  return std::make_pair(type,vers);
}

void Reader::mirror(const std::string& path, Writer& output) {
  // only mirror structure of groups
  if (getH5ObjectType(path) != HighFive::ObjectType::Group) 
    return;
  // copy over type attributes creating the group in the output file
  output.structure(path, this->type(path));
  // recurse into subobjects
  for (auto& subgrp : this->list(path)) mirror(path+"/"+subgrp, output);
}

void Reader::copy(unsigned int long i_entry, const std::string& path, Writer& output) {
  if (mirror_objects_.find(path) == mirror_objects_.end()) {
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
    mirror(path, output);
    mirror_objects_.emplace(std::make_pair(path,
          std::make_unique<MirrorObject>(path, *this)));
  }
  // do the copying
  mirror_objects_[path]->copy(i_entry, 1, output);
}

Reader::MirrorObject::MirrorObject(const std::string& path, Reader& reader) 
  : reader_{reader} {
  if (reader_.getH5ObjectType(path) == HighFive::ObjectType::Dataset) {
    // simple atomic event object
    //  unfortunately, I can't think of a better solution than manually
    //  copying the code for all of the types
    HighFive::DataType type = reader_.getDataSetType(path);
    if (type == HighFive::create_datatype<int>()) {
      data_ = std::make_unique<Branch<int>>(path);
    } else if (type == HighFive::create_datatype<long int>()) {
      data_ = std::make_unique<Branch<long int>>(path);
    } else if (type == HighFive::create_datatype<long long int>()) {
      data_ = std::make_unique<Branch<long long int>>(path);
    } else if (type == HighFive::create_datatype<unsigned int>()) {
      data_ = std::make_unique<Branch<unsigned int>>(path);
    } else if (type == HighFive::create_datatype<unsigned long int>()) {
      data_ = std::make_unique<Branch<unsigned long int>>(path);
    } else if (type == HighFive::create_datatype<unsigned long long int>()) {
      data_ = std::make_unique<Branch<unsigned long long int>>(path);
    } else if (type == HighFive::create_datatype<float>()) {
      data_ = std::make_unique<Branch<float>>(path);
    } else if (type == HighFive::create_datatype<double>()) {
      data_ = std::make_unique<Branch<double>>(path);
    } else if (type == HighFive::create_datatype<std::string>()) {
      data_ = std::make_unique<Branch<std::string>>(path);
    } else if (type == HighFive::create_datatype<hdtree::Bool>()) {
      data_ = std::make_unique<Branch<bool>>(path);
    } else {
      throw std::runtime_error("HDTreeUnknownDS: Unable to deduce C++ type "
          "from H5 type during a copy");
    }
  } else {
    // event object is a H5 group meaning it is more complicated
    // than a simple atomic type
    auto subobjs = reader_.list(path);
    for (auto& subobj : subobjs) {
      std::string sub_path{path + "/" + subobj};
      if (subobj == constants::SIZE_NAME) {
        size_member_ = std::make_unique<Branch<std::size_t>>(sub_path);
      } else {
        obj_members_.emplace_back(std::make_unique<MirrorObject>(sub_path, reader_));
      }
    }
  }
}

void Reader::MirrorObject::copy(unsigned long int i_entry, unsigned long int n, Writer& output) {
  unsigned long int num_to_advance{i_entry <= last_entry_ ? 0 : i_entry - last_entry_ - 1}, 
                    num_to_save{n};
  last_entry_ = i_entry;

  // if we have a data member, the data member is the only part of this
  // mirror object
  if (data_) {
    // load until one before desired entry
    for (std::size_t i{0}; i < num_to_advance; i++) data_->load(reader_);
    // load and save desired entries
    for (std::size_t i{0}; i < num_to_save; i++) {
      data_->load(reader_);
      data_->save(output);
    }
    return;
  }

  /// if there is a member determining the size of each entry,
  /// we need to follow its lead
  if (size_member_) {
    unsigned long int new_num_to_advance{0};
    for (std::size_t i{0}; i < num_to_advance; i++) {
      size_member_->load(reader_);
      new_num_to_advance += dynamic_cast<Branch<std::size_t>&>(*size_member_).get();
    }
    unsigned long int new_num_to_save = 0;
    for (std::size_t i{0}; i < num_to_save; i++) {
      size_member_->load(reader_);
      new_num_to_save += dynamic_cast<Branch<std::size_t>&>(*size_member_).get();
      size_member_->save(output);
    }

    num_to_advance = new_num_to_advance;
    num_to_save = new_num_to_save;
  }

  for (auto& obj  : obj_members_) obj->copy(num_to_advance, num_to_save, output);
}

}  // namespace hdtree

