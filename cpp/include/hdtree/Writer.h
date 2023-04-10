#pragma once

#include <boost/core/demangle.hpp>
#include <utility>

// using HighFive
#include <highfive/H5File.hpp>

#include "hdtree/Atomic.h"
#include "hdtree/Constants.h"
#include "hdtree/Exception.h"

namespace hdtree {

/**
 * Write the fire DataSets into a deterministic structure
 * in the output HDF5 data file.
 *
 * @see h5::Reader for where our files are read
 */
class Writer {
 public:
  /**
   * Open the file in write mode
   *
   * Our write mode is the HDF5 TRUNC (overwrite) mode.
   *
   * @param[in] event_limit Maximum number of events that could end up here
   * @param[in] ps Parameters used to configure the writing of the output file
   */
  Writer(const std::pair<std::string, std::string>& file_tree_path,
         bool inplace = false, int rows_per_chunk = 10000, bool shuffle = true,
         int compression_level = 6);

  /**
   * Close up our file, making sure to flush contents to disk
   */
  ~Writer();

  /**
   * get rows per chunk
   */
  int getRowsPerChunk() const { return rows_per_chunk_; }

  /**
   * Flush the data to disk
   *
   * We flush all buffers and then flush the file.
   */
  void flush();

  /**
   * increment the number of entries in the HDTree
   */
  void increment();

  /**
   * Get the name of this file
   */
  const std::string& name() const;

  /**
   * Persist the structure of the event object at the input branch_name
   *
   * The "structure" is simply the type and the type's version at
   * the correct location within the tree
   *
   * @note The save method creates the DataSets so this method
   * should only be called on levels of the hierarchy that DO NOT
   * correspond to HDF5 data sets.
   *
   * @param[in] branch_name branch_name to the group
   * @param[in] {type, version} pair demangled type name of object and its
   * version number
   * @param[in] version version number of type
   */
  void structure(const std::string& branch_name,
                 const std::pair<std::string, int>& type);

  HighFive::DataSet createDataSet(const std::string& branch_name,
                                  HighFive::DataType data_type);

  /**
   * Get the number of entries in the file
   */
  inline std::size_t entries() const { return entries_; }

  /**
   * Stream this writer
   *
   * We stream 'Writer' and the name of the file we are writing to
   *
   * @param[in] s ostream to stream into
   * @param[in] w writer to stream
   * @return modified ostream
   */
  friend std::ostream& operator<<(std::ostream& s, const Writer& w) {
    return s << "Writer(" << w.name() << ")";
  }

  /// don't allow copies of the Writer
  Writer(const Writer&) = delete;
  /// don't allow copies of the Writer
  void operator=(const Writer&) = delete;

 private:
  /**
   * our highfive file
   */
  HighFive::File file_;
  /**
   * handle to the HighFive group we will make be an HDTree
   */
  HighFive::Group tree_;
  /// the creation properties to be used on datasets we are writing
  HighFive::DataSetCreateProps create_props_;
  /// the dataspace shared amongst all of our datasets
  HighFive::DataSpace space_;
  /// the expected number of entries in this file
  std::size_t entries_;
  /// number of rows to keep in each chunk
  std::size_t rows_per_chunk_;
};

}  // namespace hdtree
