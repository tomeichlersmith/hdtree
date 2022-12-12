#pragma once

#include "hdtree/Branch.h"

namespace hdtree {

/**
 * A container for many branches
 */
class Tree {
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
   * Get a branch from the tree
   *
   * If we are writing and there isn't a branch matching the input name,
   * we create it. If we aren't writing and there isn't a branch matching,
   * the parameter 'create' is checked.
   */
  template<typename DataType>
  Branch<DataType>& branch(const std::string& branch_name, bool create = false) {
    auto it{branches_.find(branch_name)};
    if (it == branches_.end()) {
      if (writer_ or create) {
        branches_[branch_name] = std::make_unique<Branch<DataType>>(
            branch_name, reader_.get());
        it = branches_.find(branch_name);
      } else {
        throw std::runtime_error("No branch named '"+branch_name+"'.");
      }
    }
    return it->second;
  }

  /**
   * loop over all entries in the tree, executing the provided
   * function on each call
   *
   * tree.branch<int>("two_i_entry");
   * tree.for_each([&tree]() {
   *   tree["two_i_entry"] = 2*tree["i_entry"];
   * }, entries = 100);
   */
  template<class UnaryFunction>
  void for_each(UnaryFunction body, std::optional<std::size_t> entries = {}) {
    if (not entries_ and not entries) {
      throw std::runtime_error("I don't know how many entries to loop for.");
    } else if (entries) {
      entries_ = entries.value();
    }
    for (std::size_t i{0}; i < this->entries_; i++) {
      this->load();
      body();
      this->save();
    }
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
