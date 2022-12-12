/**
 * @file common.h
 * common tasks for testing the Branch structure
 */

#ifndef HDTREE_TEST_BRANCH_COMMON_H
#define HDTREE_TEST_BRANCH_COMMON_H

#include "hdtree/Branch.h"

template <typename ArbitraryBranch, typename DataType>
bool save(ArbitraryBranch& h5d, const DataType& d, hdtree::Writer& f) {
  try {
    *h5d = d;
    h5d.save(f);
    return true;
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
    return false;
  }
}

template <typename ArbitraryBranch, typename DataType>
bool load(ArbitraryBranch& h5d, const DataType& d, hdtree::Reader& f) {
  try {
    h5d.load(f);
    return (d == h5d.get());
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
    return false;
  }
}

#endif 
