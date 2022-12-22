#include "hdtree/Tree.h"

namespace hdtree {

Tree Tree::load(const std::string& file_path, const std::string& tree_path) {
  return Tree({file_path, tree_path}, {"", ""});
}

Tree Tree::save(const std::string& file_path, const std::string& tree_path) {
  return Tree({"",""},{file_path,tree_path});
}

Tree Tree::inplace(const std::string& file_path, const std::string& tree_path) {
  return Tree({file_path,tree_path},{file_path,tree_path});
}

Tree Tree::transform(const std::pair<std::string,std::string>& src, 
                     const std::pair<std::string,std::string>& dest) {
  if (src.first == dest.first) {
    throw std::runtime_error("Cannot transform a HDTree in the same file.\n"
                             "Are you looking for hdtree::Tree::inplace?");
  }
  return Tree(src, dest);
}

void Tree::save() {
  for (auto& [_name, br] : branches_) {
    br->save();
    br->clear();
  }
  if (writer_) writer_->increment();
}

void Tree::load() {
  for (auto& [_name, br] : branches_) br->load();
}

Tree::Tree(const std::pair<std::string,std::string>& src,
           const std::pair<std::string,std::string>& dest) {
  bool reading = (not src.first.empty());
  bool writing = (not dest.first.empty());
  bool inplace_ = (src.first == dest.first);

  if (inplace_ and src.first != dest.first) {
    throw std::runtime_error("Tree does not support copying an HDTree to a new location within the same file."); 
  }

  if (reading) {
    reader_ = std::make_unique<Reader>(src, inplace_);
    entries_ = reader_->entries();
  }

  if (writing) {
    writer_ = std::make_unique<Writer>(dest, inplace_);
  }
}

}
