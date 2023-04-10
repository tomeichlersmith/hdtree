#pragma once

#include "hdtree/Branch.h"

namespace hdtree {

/**
 * A container for many branches
 */
class Tree {
 public:
  static Tree load(const std::string& file_path, const std::string& tree_path);
  static Tree save(const std::string& file_path, const std::string& tree_path);
  static Tree inplace(const std::string& file_path,
                      const std::string& tree_path);
  static Tree transform(const std::pair<std::string, std::string>& src,
                        const std::pair<std::string, std::string>& dest);

  /**
   * Create a new branch on the tree
   */
  template <typename DataType>
  Branch<DataType>& branch(const std::string& branch_name) {
    if (branches_.find(branch_name) != branches_.end()) {
      throw HDTreeException(
          "Branch named '" + branch_name + "' was already initialized.",
          "This usually originates from more than one call to `tree.branch` "
          "and/or `tree.get` with the same input branch name.");
    }
    if (not writer_) {
      throw HDTreeException(
          "Attempting to sprout a new branch without writing.",
          "While it could make sense to have a \"tracking\" branch that "
          "only exists in memory, HDTree has decided to not support that.\n"
          "You should not call `tree.branch` on a tree that is not going "
          "to be writing its data into an output file.");
    }
    branches_[branch_name] = std::make_unique<Branch<DataType>>(branch_name);
    branches_[branch_name]->attach(*writer_);
    return dynamic_cast<Branch<DataType>&>(*branches_[branch_name]);
  }

  /**
   * get a branch, this only really makes sense if reading
   */
  template <typename DataType>
  const Branch<DataType>& get(const std::string& branch_name,
                              bool write = false) {
    if (not reader_) {
      throw HDTreeException(
          "Attempting to 'get' a branch without reading.",
          "There is no reference for a tree to know how to retrieve a "
          "branch for you if there is no input file to try to get a branch "
          "from. Make sure you've loaded a tree if you wish to 'get' a branch."
          );
    }
    if (branches_.find(branch_name) != branches_.end()) {
      throw HDTreeException(
          "Branch named '" + branch_name + "' was already initialized.",
          "This usually originates from more than one call to `tree.branch` "
          "and/or `tree.get` with the same input branch name.");
    }
    branches_[branch_name] = std::make_unique<Branch<DataType>>(branch_name);
    branches_[branch_name]->attach(*reader_);
    if (not inplace_ and writer_ and write)
      branches_[branch_name]->attach(*writer_);
    return dynamic_cast<Branch<DataType>&>(*branches_[branch_name]);
  }

  /**
   * loop over all entries in the tree, executing the provided
   * function on each call
   *
   * auto& i_entry = tree.branch<int>("i_entry");
   * auto& two_i_entry = tree.branch<int>("two_i_entry");
   * tree.for_each([&]() {
   *   *two_i_entry = 2*(*i_entry);
   * });
   */
  template <class UnaryFunction>
  void for_each(UnaryFunction body) {
    if (not reader_) {
      throw HDTreeException(
          "No reader configured, so I don't know how many entries to loop for.",
          "While the 'for_each' helper function is useful for trees being read "
          "from a file, WITHOUT an input file, YOU must decide how many entries "
          "will be in the tree.\n"
          "REMEMBER: make sure to call tree.save() whenever you want an entry "
          "to be stored (for example at the end of the body of a for-loop)."
          );
    }
    for (std::size_t i{0}; i < this->entries_; i++) {
      this->load();
      body();
      this->save();
    }
  }

  /**
   * end-of-event call back
   *
   * basically, we go through and save+clear each branch that is in this tree
   *
   * The 'clear' call is helpful so users can treat their handle to a Branch
   * as if it is a local variable in the loop.
   *
   * At the end, we also inform the writer (if there is one) that the number of
   * entries in the tree has incremented.
   */
  void save();

  /**
   * start-of-event call back
   *
   * we go through and load each branch that is in this tree
   */
  void load();

 private:
  /**
   * The tree constructor is private because it is complicated,
   * use the static factory functions for accessing a Tree
   */
  Tree(const std::pair<std::string, std::string>& src,
       const std::pair<std::string, std::string>& dest);

 private:
  /// the branches in this tree
  std::unordered_map<std::string, std::unique_ptr<BaseBranch>> branches_;
  /// the number of entries in this tree (if reading from a file)
  std::optional<std::size_t> entries_;
  /// reader if loading from a file
  std::unique_ptr<Reader> reader_;
  /// writer if writing to a file
  std::unique_ptr<Writer> writer_;
  /// are we reading from and writing to the same file?
  bool inplace_{false};
};

}  // namespace hdtree
