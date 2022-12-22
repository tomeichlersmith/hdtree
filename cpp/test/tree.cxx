/**
 * @file branch.cxx
 *
 * This test makes sure the recursive nature of the Branch class is functional,
 * and all different types of data from atomic types to containers of user
 * classes are serializable.
 */
#include <boost/test/tools/interface.hpp>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <highfive/H5Easy.hpp>

#include "hdtree/Tree.h"

static std::string filename{"tree.h5"};

static std::vector<double> doubles = { 1.0, 32., 69. };

BOOST_AUTO_TEST_SUITE(tree)

BOOST_AUTO_TEST_CASE(write) {
  hdtree::Tree t = hdtree::save(filename,"test");
  auto& b = t.branch<double>("double");

  for (std::size_t i{0}; i < doubles.size(); ++i) {
    *b = doubles.at(i);
    t.save();
  }
}

/*
BOOST_AUTO_TEST_CASE(inplace, *boost::unit_test::depends_on("tree/write")) {
  hdtree::Tree t = hdtree::inplace(filename, "test");
  auto& b = t.get<double>("double", true);
  auto& b2 = t.branch<double>("double_sq");

  BOOST_CHECK_NO_THROW(
      t.for_each([&]() {
          *b2 = (*b) * (*b);
        }));
}
*/

BOOST_AUTO_TEST_CASE(copy, *boost::unit_test::depends_on("tree/write")) {
  hdtree::Tree t(filename,"test","copy_"+filename,"test2");
  const auto& b = t.get<double>("double", true);
  auto& b2 = t.branch<double>("double_sq");

  BOOST_CHECK_NO_THROW(
      t.for_each([&]() {
        *b2 = b.get()*b.get();
        }));
}

BOOST_AUTO_TEST_CASE(read, *boost::unit_test::depends_on("tree/copy")) {
  hdtree::Tree t = hdtree::load("copy_"+filename,"test2");
  auto& b = t.get<double>("double");
  auto& b2 = t.get<double>("double_sq");

  std::size_t i{0};
  t.for_each([&]() {
        double v = doubles.at(i++);
        // the * de-reference is just syntax sugar for get
        BOOST_CHECK(b.get() == v);
        BOOST_CHECK(*b2 == v*v);
      });
}

BOOST_AUTO_TEST_SUITE_END()
