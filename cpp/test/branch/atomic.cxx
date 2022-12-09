#include <boost/test/tools/interface.hpp>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <highfive/H5Easy.hpp>

#include "common.h"

static std::string filename{"branch_atomic.h5"};

static std::vector<double> double_data = { 1.0, 32., 69. };
static std::vector<int>    int_data    = { 0, -33, 88 };

BOOST_AUTO_TEST_SUITE(branch)
BOOST_AUTO_TEST_SUITE(atomic)
BOOST_AUTO_TEST_CASE(write) {
  hdtree::Writer f{filename,"test"};

  hdtree::Branch<double> double_ds("double");
  hdtree::Branch<int>    int_ds("int");
  hdtree::Branch<bool>   bool_ds("bool");

  double_ds.structure(f);
  int_ds.structure(f);
  bool_ds.structure(f);

  for (std::size_t i_entry{0}; i_entry < double_data.size(); i_entry++) {
    BOOST_CHECK(save(double_ds,double_data.at(i_entry),f));
    BOOST_CHECK(save(int_ds,int_data.at(i_entry),f));
    bool positive{int_data.at(i_entry) > 0};
    BOOST_CHECK(save(bool_ds,positive,f));
    f.increment();
  }
}

BOOST_AUTO_TEST_CASE(read, *boost::unit_test::depends_on("branch/atomic/write")) {
  hdtree::Reader f{filename,"test"};

  BOOST_CHECK(f.entries() == double_data.size());

  hdtree::Branch<double> double_ds("double", &f);
  hdtree::Branch<int>    int_ds("int", &f);
  hdtree::Branch<bool>   bool_ds("bool", &f);

  for (std::size_t i_entry{0}; i_entry < double_data.size(); i_entry++) {
    BOOST_CHECK(load(double_ds,double_data.at(i_entry),f));
    BOOST_CHECK(load(int_ds,int_data.at(i_entry),f));
    bool positive{int_data.at(i_entry) > 0};
    BOOST_CHECK(load(bool_ds,positive,f));
  }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
