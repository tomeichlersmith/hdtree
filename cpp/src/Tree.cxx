#include "hdtree/Tree.h"

namespace hdtree {

Tree Tree::load(const std::string& file_path, const std::string& tree_path) {
  return Tree({file_path, tree_path}, {"", ""});
}

Tree Tree::save(const std::string& file_path, const std::string& tree_path) {
  return Tree({"", ""}, {file_path, tree_path});
}

Tree Tree::inplace(const std::string& file_path, const std::string& tree_path) {
  return Tree({file_path, tree_path}, {file_path, tree_path});
}

Tree Tree::transform(const std::pair<std::string, std::string>& src,
                     const std::pair<std::string, std::string>& dest) {
  if (src.first == dest.first) {
    throw HDTreeException(
        "Cannot transform a HDTree in the same file.",
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

Tree::Tree(const std::pair<std::string, std::string>& src,
           const std::pair<std::string, std::string>& dest) {
  bool reading = (not src.first.empty());
  bool writing = (not dest.first.empty());
  bool inplace_ = (src.first == dest.first);

  if (inplace_ and src.first != dest.first) {
    throw HDTreeException(
        "HDTree does not support copying a HDTree to a new location within the "
        "same file.",
        "Either use two different files (`hdtree::Tree::transform`) or \"update\" "
        "an existing HDTree in place without copying already existing data "
        "(`hdtree::Tree::inplace`)."
        );
  }

  if (reading) {
    try {
      reader_ = std::make_unique<Reader>(src, inplace_);
    } catch (const HighFive::FileException& e) {
      throw hdtree::HDTreeException(
          "File '"+src.first + "' is not accessible."
          );
    } catch (const HighFive::GroupException& e) {
      throw hdtree::HDTreeException(
          "HDTree '" src.second + "' does not exist within '" + src.first + "'."
          );
    }
    entries_ = reader_->entries();
  }

  if (writing) {
    try {
      writer_ = std::make_unique<Writer>(dest, inplace_);
    } catch (const HighFive::FileException& e) {
      throw hdtree::HDTreeException(
          "File '"+dest.first+"' is not write-able."
          );
    } catch (const HighFive::GroupException& e) {
      std::stringstream msg;
      msg << "HDTree '" << dest.second << "' ";
      if (inplace) msg << "does not exist";
      else msg << "already exists";
      msg << " within '" << dest.first << "'.";
      throw hdtree::HDTreeException(msg.str());
    }
  }
}

}  // namespace hdtree
