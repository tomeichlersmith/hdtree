#pragma once

#include "hdtree/Branch.h"

namespace hdtree {

/**
 * A container for many branches
 */
class Tree {
 public:
  /**
   * Single-file access: either opening a file to read it or opening one to write it
   */
  Tree(const std::string& file_path, const std::string& tree_path, 
       bool writing = false, bool inplace = false);

  /**
   * Two-file access: reading from one and writing to another
   */
  Tree(const std::string& src_file_path, const std::string& src_tree_path,
       const std::string& dest_file_path, const std::string& dest_tree_path);

  /**
   * Create a new branch on the tree
   */
  template<typename DataType>
  Branch<DataType>& branch(const std::string& branch_name) {
    if (branches_.find(branch_name) != branches_.end()) {
      throw std::runtime_error("Branch named '"+branch_name+"' was already initialized.");
    }
    if (not writer_) {
      throw std::runtime_error("Attmempting to sprout a new branch without writing.");
    }
    branches_[branch_name] = std::make_unique<Branch<DataType>>(branch_name);
    branches_[branch_name]->attach(*writer_);
    return dynamic_cast<Branch<DataType>&>(*branches_[branch_name]);
  }

  /**
   * get a branch, this only really makes sense if reading
   */
  template<typename DataType>
  const Branch<DataType>& get(const std::string& branch_name, bool write = false) {
    if (not reader_) {
      throw std::runtime_error("Attempting to 'get' a branch without reading.");
    }
    if (branches_.find(branch_name) != branches_.end()) {
      throw std::runtime_error("Branch named '"+branch_name+"' was already initialized.");
    }
    branches_[branch_name] = std::make_unique<Branch<DataType>>(branch_name);
    branches_[branch_name]->attach(*reader_);
    if (writer_ and write) branches_[branch_name]->attach(*writer_);
    return dynamic_cast<Branch<DataType>&>(*branches_[branch_name]);
  }

  /**
   * loop over all entries in the tree, executing the provided
   * function on each call
   *
   * auto& i_entry = tree.branch<int>("i_entry");
   * auto& two_i_entry = tree.branch<int>("two_i_entry");
   * tree.for_each([&tree]() {
   *   *two_i_entry = 2*(*i_entry);
   * }, 100);
   */
  template<class UnaryFunction>
  void for_each(UnaryFunction body) {
    if (not reader_) {
      throw std::runtime_error("No reader configured, so I don't know how many entries to loop for.");
    }
    for (std::size_t i{0}; i < this->entries_; i++) {
      this->load();
      body();
      this->save();
    }
  }

  void save() {
    if (not writer_) return;
    for (auto& [_name, br] : branches_) br->save();
    writer_->increment();
  }

  void load() {
    if (not reader_) return;
    for (auto& [_name, br] : branches_) br->load();
  }

 private:
  /// the branches in this tree
  std::unordered_map<std::string, std::unique_ptr<BaseBranch>> branches_;
  /// the number of entries in this tree (if reading from a file)
  std::optional<std::size_t> entries_;
  /// reader if loading from a file
  std::unique_ptr<Reader> reader_;
  /// writer if writing to a file
  std::unique_ptr<Writer> writer_;
};

/**
 * reading single file
 */
Tree load(const std::string& file_path, const std::string& tree_path) {
  return Tree(file_path, tree_path, false, false);
}

/**
 * writing to a single file
 */
Tree save(const std::string& file_path, const std::string& tree_path) {
  return Tree(file_path, tree_path, true, false);
}

/**
 * modifying a file in place
 */
Tree inplace(const std::string& file_path, const std::string& tree_path) {
  return Tree(file_path, tree_path, true, true);
}

}
