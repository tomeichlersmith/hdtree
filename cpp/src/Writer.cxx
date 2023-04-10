#include "hdtree/Writer.h"

#include "hdtree/Constants.h"
#include "hdtree/Version.h"

namespace hdtree {

Writer::Writer(const std::pair<std::string, std::string>& file_tree_path,
               bool inplace, int rows_per_chunk, bool shuffle,
               int compression_level)
    : file_{file_tree_path.first,
            inplace ? HighFive::File::ReadWrite
                    : (HighFive::File::Create | HighFive::File::Truncate)},
      tree_{inplace ? file_.getGroup(file_tree_path.second)
                    : file_.createGroup(file_tree_path.second)},
      create_props_{},
      space_(std::vector<std::size_t>({0}),
             std::vector<std::size_t>({HighFive::DataSpace::UNLIMITED})),
      entries_{0} {
  rows_per_chunk_ = rows_per_chunk;
  // copy creation properties into HighFive structure
  create_props_.add(HighFive::Chunking({rows_per_chunk_}));
  if (shuffle) create_props_.add(HighFive::Shuffle());
  create_props_.add(HighFive::Deflate(compression_level));

  if (not inplace) {
    tree_.createAttribute(constants::VERS_ATTR_NAME, 1 /*HDTREE_VERSION*/);
    tree_.createAttribute("__api__", API());
    tree_.createAttribute("__api_version__", VERSION());
  }
}

Writer::~Writer() { this->flush(); }

void Writer::flush() {
  if (tree_.hasAttribute(constants::SIZE_NAME)) {
    tree_.getAttribute(constants::SIZE_NAME).write(entries_);
  } else {
    tree_.createAttribute(constants::SIZE_NAME, entries_);
  }
  file_.flush();
}

const std::string& Writer::name() const { return file_.getName(); }

void Writer::increment() { entries_++; }

void Writer::structure(const std::string& branch_name,
                       const std::pair<std::string, int>& type) {
  if (tree_.exist(branch_name)) {
    // group already been written to, check that we are the same
    auto grp = tree_.getGroup(branch_name);
    int v;
    grp.getAttribute(constants::VERS_ATTR_NAME).read(v);
    std::string t;
    grp.getAttribute(constants::TYPE_ATTR_NAME).read(t);
    if (t != type.first or v != type.second) {
      std::stringstream ss;
      ss << "HDTreeMisType: Attempting to write multiple types "
         << "(or versions of a type) as the same output data.\n"
         << "     Branch: " << branch_name << "\n"
         << "     Type Already on Disk: " << t << " (version " << v << ")\n"
         << "     Type to Write: " << type.first << " (version " << type.second
         << ")";
      throw HDTreeException(ss.str());
    }
  } else {
    // group structure doesn't exist yet, put in this type
    auto grp = tree_.createGroup(branch_name);
    grp.createAttribute(constants::TYPE_ATTR_NAME, type.first);
    grp.createAttribute(constants::VERS_ATTR_NAME, type.second);
  }
}

HighFive::DataSet Writer::createDataSet(const std::string& branch_name,
                                        HighFive::DataType data_type) {
  return tree_.createDataSet(branch_name, space_, data_type, create_props_);
}

}  // namespace hdtree
