#include <boost/test/tools/interface.hpp>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <highfive/H5Easy.hpp>

#include "common.h"

static std::string filename{"branch_atomic_vector.h5"};

static std::vector<std::vector<double>> double_data = {
  { 1.0, 32., 69. },
  { 0.0 },
  { },
  { -10, 1e-4 }
};

BOOST_AUTO_TEST_SUITE(branch)
BOOST_AUTO_TEST_SUITE(atomic_vector)
BOOST_AUTO_TEST_CASE(write) {
  hdtree::Writer f{filename,"test"};

  hdtree::Branch<std::vector<double>> double_ds("double");
  hdtree::Branch<std::vector<int>>    int_ds("int");
  hdtree::Branch<std::vector<bool>>   bool_ds("bool");
  hdtree::Branch<std::vector<std::string>> str_ds("string");

  double_ds.structure(f);
  int_ds.structure(f);
  bool_ds.structure(f);
  str_ds.structure(f);

  for (std::size_t i_entry{0}; i_entry < double_data.size(); i_entry++) {
    BOOST_CHECK(save(double_ds,double_data.at(i_entry),f));
    std::vector<int> int_data(double_data.at(i_entry).size());
    for (const double& d : double_data.at(i_entry)) {
      int_data.push_back(int(d));
    }
    BOOST_CHECK(save(int_ds,int_data,f));
    std::vector<bool> positive(int_data.size());
    std::vector<std::string> str_data(int_data.size());
    for (const int& i : int_data) {
      positive.push_back(i > 0);
      str_data.push_back(std::to_string(i));
    }
    BOOST_CHECK(save(bool_ds,positive,f));
    BOOST_CHECK(save(str_ds, str_data, f));
    f.increment();
  }
}

BOOST_AUTO_TEST_CASE(read, *boost::unit_test::depends_on("branch/atomic_vector/write")) {
  hdtree::Reader f{filename,"test"};

  BOOST_CHECK(f.entries() == double_data.size());

  hdtree::Branch<std::vector<double>> double_ds("double", &f);
  hdtree::Branch<std::vector<int>>    int_ds("int", &f);
  hdtree::Branch<std::vector<bool>>   bool_ds("bool", &f);
  hdtree::Branch<std::vector<std::string>> str_ds("string", &f);

  for (std::size_t i_entry{0}; i_entry < double_data.size(); i_entry++) {
    BOOST_CHECK(load(double_ds,double_data.at(i_entry),f));
    std::vector<int> int_data(double_data.at(i_entry).size());
    for (const double& d : double_data.at(i_entry)) {
      int_data.push_back(int(d));
    }
    BOOST_CHECK(load(int_ds,int_data,f));
    std::vector<bool> positive(int_data.size());
    std::vector<std::string> str_data(int_data.size());
    for (const int& i : int_data) {
      positive.push_back(i > 0);
      str_data.push_back(std::to_string(i));
    }
    BOOST_CHECK(load(bool_ds,positive,f));
    BOOST_CHECK(load(str_ds, str_data, f));
  }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
