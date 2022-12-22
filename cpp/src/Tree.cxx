#include "hdtree/Tree.h"

namespace hdtree {

Tree::Tree(const std::string& file_path, const std::string& tree_path,
           bool writing, bool inplace)
  : inplace_{inplace} {
  if (not writing) reader_ = std::make_unique<Reader>(file_path,tree_path);
  if (writing) {
    if (inplace_) reader_ = std::make_unique<Reader>(file_path,tree_path, inplace_);
    writer_ = std::make_unique<Writer>(file_path,tree_path, inplace_);
  }

  if (reader_) entries_ = reader_->entries();
}

Tree::Tree(const std::string& src_file_path, const std::string& src_tree_path,
           const std::string& dest_file_path, const std::string& dest_tree_path) {
  reader_ = std::make_unique<Reader>(src_file_path, src_tree_path);
  writer_ = std::make_unique<Writer>(dest_file_path, dest_tree_path);
  entries_ = reader_->entries();
}

}
