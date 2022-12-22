#pragma once

// using HighFive
#include <highfive/H5File.hpp>

#include "hdtree/Atomic.h"
#include "hdtree/AbstractBranch.h"
#include "hdtree/Writer.h"

namespace hdtree {

/**
 * Reading a file generated by fire
 *
 * @see Writer for our files are written with fire
 *
 * This reader handles the buffering of data read in from an HDF5 file
 * in a seamless manner so that individual entries can be requested at
 * a time without making disk read operation each time Reader::load is
 * called.
 */
class Reader {
 public:
  /**
   * Open the file in read mode
   *
   * Our read mode is the same as HDF5 Read Only mode
   *
   * We also read the number of entries in this file by 
   * retrieving __size__ attribute of the HDTree.
   * This is reliable as long as the HDTree was written/flushed
   * before program termination.
   *
   * @throws HighFive::Exception if file is not accessible.
   * @param[in] name file name to open and read
   */
  Reader(const std::string& file_path, const std::string& tree_path, bool inplace = false);

  /**
   * Get the event objects available in the file
   *
   * We crawl the internal directory structure of the file we have open.
   *
   * @see type for how we get the name of the class that was used
   * to write the data
   *
   * @return vector of string pairs `{ obj_name, pass }`
   */
  virtual std::vector<std::pair<std::string,std::string>> availableObjects();

  /**
   * Get the type of the input object
   *
   * We retrieve the attributes named TYPE_ATTR_NAME and VERS_ATTR_NAME 
   * from the group located at branch_name within the tree.
   *
   * @see io::Writer::setTypeName for where the type attr is set
   *
   * @param[in] branch_name Name of event object to retrieve type of
   * @return demangled type name in string format and its version number
   */
  virtual std::pair<std::string,int> type(const std::string& branch_name);

  /**
   * Get the name of this file
   * @return name of the file we are reading
   */
  virtual std::string name() const;

  /**
   * List the entries inside of a group.
   *
   * @note This is low-level and is only being used in 
   * io::ParameterStorage for more flexibility in the parameter maps
   * and Event::setInputFile to obtain the event products within a file.
   *
   * @param[in] group_path full in-file path to H5 group to list elements of
   * @return list of elements in the group, empty if group does not exist
   */
  std::vector<std::string> list(const std::string& group_path) const;

  /**
   * Get data set
   */
  HighFive::DataSet getDataSet(const std::string& branch_name) const;

  /**
   * Deduce the type of the dataset requested.
   *
   * @note This is low-level and is only being used in io::ParameterStorage
   * for more flexibility in the parameter maps.
   *
   * @param[in] dataset full in-file path to H5 dataset
   * @return HighFive::DataType specifying the atomic type of the set
   */
  HighFive::DataType getDataSetType(const std::string& dataset) const;

  /**
   * Get the H5 type of object at the input path
   * @param[in] path in-file path to an HDF5 object
   * @return HighFive::ObjectType defining the type that the object there is
   */
  HighFive::ObjectType getH5ObjectType(const std::string& path) const;

  /**
   * Get the 'type' attribute from the input group name
   *
   * We retrieve the attribute named TYPE_ATTR_NAME from the group
   * located at EVENT_GROUP/obj_name. This enables us to construct
   * the list of products within an inputfile in Event::setInputFile.
   *
   * @see io::Writer::setTypeName for where the type attr is set
   *
   * @param[in] obj_name Name of event object to retrieve type of
   * @return demangled type name in string format
   */
  std::string getTypeName(const std::string& obj_name) const;

  /**
   * Get the number of entries in the file
   *
   * This value was determined upon construction.
   *
   * @return number of events within the file
   */
  inline std::size_t entries() const { return entries_; }

  /**
   * We can copy
   * @return true
   */
  virtual bool canCopy() const { return true; }

  /**
   * Copy the input data set to the output file
   *
   * This happens when the input data set has passed all of the drop/keep
   * rules so it is supposed to be copied into the output file; however,
   * noone has accessed it with Event::get yet so an in-memory class object
   * has not been created for it yet.
   *
   * @param[in] i_entry entry we are currently on
   * @param[in] branch_name full event object name
   * @param[in] output handle to the writer writing the output file
   */
  virtual void copy(unsigned long int i_entry, const std::string& branch_name, 
      Writer& output);

  /// never want to copy a reader
  Reader(const Reader&) = delete;
  /// never want to copy a reader
  void operator=(const Reader&) = delete;

 private:
  /**
   * Mirror the structure of the passed branch_name from us into the output file
   *
   * @param[in] branch_name path to the group (and potential subgroups) to mirror
   * @param[in] output output file to mirror this structure to
   */
  void mirror(const std::string& branch_name, Writer& output);

 private:
  /**
   * A mirror event object
   *
   * This type of event object is merely present to "reflect" (pun intended)
   * the recursive nature of the Branch pattern _without_ knowledge of
   * any user classes. We need this so we can effectively copy event objects
   * that are not accessed during processing from the input to the output
   * file. (The choice on whether to copy or not copy these files is 
   * handled by Event).
   */
  class MirrorObject {
    /// handle to the atomic data type once we get down to that point
    std::unique_ptr<BaseBranch> data_;
    /// handle to the size member of this object (if it exists)
    std::unique_ptr<BaseBranch> size_member_;
    /// list of sub-objects within this object
    std::vector<std::unique_ptr<MirrorObject>> obj_members_;
    /// the last entry that was copied
    unsigned long int last_entry_{0};

   public:
    /**
     * Construct this mirror object and any of its (data or object) children
     */
    MirrorObject(const std::string& branch_name, Reader& reader, Writer& writer);

    /**
     * Copy the n entries starting from i_entry
     */
    void copy(unsigned long int i_entry, unsigned long int n);
  };

 private:
  /// our highfive file
  HighFive::File file_;
  /// the HighFive group that is our HDTree
  HighFive::Group tree_;
  /// the number of entries in this file, set in constructor
  std::size_t entries_;
  /// our in-memory mirror objects for data being copied to the output file without processing
  std::unordered_map<std::string, std::unique_ptr<MirrorObject>> mirror_objects_;
};  // Reader

}  // namespace hdtree

