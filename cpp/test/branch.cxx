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

#include "hdtree/Branch.h"

// plain old data class
class Hit {
  double energy_;
  int id_;
 protected:
  friend class hdtree::access;
  template<typename DataSet>
  void attach(DataSet& d) {
    d.attach("energy",energy_);
    d.attach("id",id_);
  }
 public:
  Hit() = default;
  Hit(double e, int id) : energy_{e}, id_{id} {}
  bool operator==(Hit const& other) const {
    return energy_ == other.energy_ and id_ == other.id_;
  }
  void clear() {
    energy_ = 0.;
    id_ = 0;
  }
};

// derived class
class DerivedHit : public Hit {
  double another_;
  friend class hdtree::access;
  template<typename DataSet>
  void attach(DataSet& d) {
    Hit::attach(d);
    d.attach("another", another_);
  }
 public:
  DerivedHit() = default;
  DerivedHit(double e, int id) : Hit(e,id), another_{e} {}
  bool operator==(const DerivedHit& other) const {
    return Hit::operator==(other) and another_ == other.another_;
  }
  void clear() {
    DerivedHit::clear();
    another_ = 0.;
  }

};

// class with nested class
class SpecialHit {
  int super_id_;
  Hit hit_;
 private:
  friend class hdtree::access;
  template<typename DataSet>
  void attach(DataSet& d) {
    d.attach("super_id",super_id_);
    d.attach("hit",hit_);
  }
 public:
  SpecialHit() = default;
  SpecialHit(int sid, Hit h) : super_id_{sid}, hit_{h} {}
  bool operator==(SpecialHit const& other) const {
    return super_id_ == other.super_id_ and hit_ == other.hit_;
  }
  void clear() {
    hit_.clear();
    super_id_ = 0;
  }
}; // SpecialHit

// class with vector of nested class
class Cluster {
  int id_;
  std::vector<Hit> hits_;
 private:
  friend class hdtree::access;
  template <typename DataSet>
  void attach(DataSet& d) {
    d.attach("id", id_);
    d.attach("hits",hits_);
  }
 public:
  Cluster() = default;
  Cluster(int id, std::vector<Hit> const& hits) : id_{id}, hits_{hits} {}
  bool operator==(Cluster const& other) const {
    if (id_ != other.id_) { return false; }
    if (hits_.size() != other.hits_.size()) { return false; }
    for (std::size_t i{0}; i < hits_.size(); i++) {
      if (not(hits_.at(i) == other.hits_.at(i))) {
        return false;
      }
    }
    return true;
  }
  void clear() {
    id_ = 0;
    hits_.clear();
  }
};

template <typename ArbitraryBranch, typename DataType>
bool save(ArbitraryBranch& h5d, DataType const& d) {
  try {
    *h5d = d;
    h5d.save();
    return true;
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
    return false;
  }
}

template <typename ArbitraryBranch, typename DataType>
bool load(ArbitraryBranch& h5d, DataType const& d) {
  try {
    h5d.load();
    return (d == h5d.get());
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
    return false;
  }
}

static std::string filename{"branch.h5"};
static std::string copy_file{"copy_"+filename};

static std::vector<double> doubles = { 1.0, 32., 69. };
static std::vector<int>    ints    = { 0, -33, 88 };
static std::vector<
         std::vector<Hit>
       > all_hits = {
         {Hit(25.,1), Hit(32.,2), Hit(1234.,10)},
         {Hit(25.,1), Hit(32.,2), Hit(23.,10), Hit(321.,69)},
         {Hit(2.,1), Hit(31.,6)}
       };


BOOST_AUTO_TEST_SUITE(branch)

BOOST_AUTO_TEST_CASE(write) {
  hdtree::Writer f{filename,"test"};

  hdtree::Branch<double> double_ds("double");
  hdtree::Branch<int>    int_ds("int");
  hdtree::Branch<bool>   bool_ds("bool");
  hdtree::Branch<std::string> str_ds("string");
  hdtree::Branch<std::vector<double>> vector_double_ds("vector_double");
  hdtree::Branch<std::vector<int>> vector_int_ds("vector_int");
  hdtree::Branch<std::map<int,double>> map_int_double_ds("map_int_double");
  hdtree::Branch<Hit> hit_ds("hit");
  hdtree::Branch<std::vector<Hit>> vector_hit_ds("vector_hit");
  hdtree::Branch<SpecialHit> special_hit_ds("special_hit");
  hdtree::Branch<DerivedHit> derived_hit_ds("derived_hit");
  hdtree::Branch<std::vector<SpecialHit>> vector_special_hit_ds("vector_special_hit");
  hdtree::Branch<Cluster> cluster_ds("cluster");
  hdtree::Branch<std::vector<Cluster>> vector_cluster_ds("vector_cluster");
  hdtree::Branch<std::map<int,Cluster>> map_cluster_ds("map_cluster");

  double_ds.attach(f);
  int_ds.attach(f);
  bool_ds.attach(f);
  str_ds.attach(f);
  vector_double_ds.attach(f);
  vector_int_ds.attach(f);
  map_int_double_ds.attach(f);
  hit_ds.attach(f);
  vector_hit_ds.attach(f);
  special_hit_ds.attach(f);
  derived_hit_ds.attach(f);
  vector_special_hit_ds.attach(f);
  cluster_ds.attach(f);
  vector_cluster_ds.attach(f);
  map_cluster_ds.attach(f);

  for (std::size_t i_entry{0}; i_entry < doubles.size(); i_entry++) {
    BOOST_CHECK(save(double_ds,doubles.at(i_entry)));
    BOOST_CHECK(save(int_ds,ints.at(i_entry)));
    BOOST_CHECK(save(str_ds,std::to_string(ints.at(i_entry))));

    bool positive{ints.at(i_entry) > 0};
    BOOST_CHECK(save(bool_ds,positive));

    BOOST_CHECK(save(vector_double_ds,doubles));
    BOOST_CHECK(save(vector_int_ds,ints));
    std::map<int,double> map_int_double;
    for (std::size_t i{0}; i < ints.size(); i++) {
      map_int_double[ints.at(i)] = doubles.at(i);
    }
    BOOST_CHECK(save(map_int_double_ds,map_int_double));

    BOOST_CHECK(save(hit_ds,all_hits[i_entry][0]));
    BOOST_CHECK(save(vector_hit_ds,all_hits[i_entry]));
    BOOST_CHECK(save(special_hit_ds,SpecialHit(0,all_hits[i_entry][0])));
    BOOST_CHECK(save(derived_hit_ds,DerivedHit(25., 2)));

    std::vector<SpecialHit> sphit_vec;
    for (auto& hit : all_hits[i_entry]) sphit_vec.emplace_back(i_entry,hit);
    BOOST_CHECK(save(vector_special_hit_ds,sphit_vec));

    auto c = Cluster(i_entry, all_hits.at(i_entry));
    BOOST_CHECK(save(cluster_ds,c));

    std::vector<Cluster> clusters;
    clusters.emplace_back(2, all_hits.at(0));
    clusters.emplace_back(3, all_hits.at(1));
    BOOST_CHECK(save(vector_cluster_ds,clusters));

    std::map<int,Cluster> map_clusters = {
      { 2, Cluster(2, all_hits.at(0)) },
      { 3, Cluster(3, all_hits.at(1)) }
    };
    BOOST_CHECK(save(map_cluster_ds,map_clusters));
  }
}

BOOST_AUTO_TEST_CASE(read, *boost::unit_test::depends_on("branch/write")) {
  hdtree::Reader f{filename, "test"};

  hdtree::Branch<Hit> hit_ds("hit");
  hdtree::Branch<double> double_ds("double");
  hdtree::Branch<int>    int_ds("int");
  hdtree::Branch<bool>   bool_ds("bool");
  hdtree::Branch<std::string> str_ds("string");
  hdtree::Branch<std::vector<double>> vector_double_ds("vector_double");
  hdtree::Branch<std::vector<int>> vector_int_ds("vector_int");
  hdtree::Branch<std::map<int,double>> map_int_double_ds("map_int_double");
  hdtree::Branch<std::vector<Hit>> vector_hit_ds("vector_hit");
  hdtree::Branch<SpecialHit> special_hit_ds("special_hit");
  hdtree::Branch<DerivedHit> derived_hit_ds("derived_hit");
  hdtree::Branch<std::vector<SpecialHit>> vector_special_hit_ds("vector_special_hit");
  hdtree::Branch<Cluster> cluster_ds("cluster");
  hdtree::Branch<std::vector<Cluster>> vector_cluster_ds("vector_cluster");
  hdtree::Branch<std::map<int,Cluster>> map_cluster_ds("map_cluster");

  double_ds.attach(f);
  int_ds.attach(f);
  bool_ds.attach(f);
  str_ds.attach(f);
  vector_double_ds.attach(f);
  vector_int_ds.attach(f);
  map_int_double_ds.attach(f);
  hit_ds.attach(f);
  vector_hit_ds.attach(f);
  special_hit_ds.attach(f);
  derived_hit_ds.attach(f);
  vector_special_hit_ds.attach(f);
  cluster_ds.attach(f);
  vector_cluster_ds.attach(f);
  map_cluster_ds.attach(f);

  for (std::size_t i_entry{0}; i_entry < doubles.size(); i_entry++) {
    BOOST_CHECK(load(hit_ds,all_hits[i_entry][0]));
    BOOST_CHECK(load(double_ds,doubles.at(i_entry)));
    BOOST_CHECK(load(int_ds,ints.at(i_entry)));
    BOOST_CHECK(load(str_ds,std::to_string(ints.at(i_entry))));
    bool positive{ints.at(i_entry) > 0};
    BOOST_CHECK(load(bool_ds,positive));
    BOOST_CHECK(load(vector_double_ds,doubles));
    BOOST_CHECK(load(vector_int_ds,ints));
    std::map<int,double> map_int_double;
    for (std::size_t i{0}; i < ints.size(); i++) {
      map_int_double[ints.at(i)] = doubles.at(i);
    }
    BOOST_CHECK(load(map_int_double_ds,map_int_double));

    BOOST_CHECK(load(vector_hit_ds,all_hits[i_entry]));
    BOOST_CHECK(load(special_hit_ds,SpecialHit(0,all_hits[i_entry][0])));
    BOOST_CHECK(load(derived_hit_ds,DerivedHit(25.,2)));

    std::vector<SpecialHit> sphit_vec;
    for (auto& hit : all_hits[i_entry]) sphit_vec.emplace_back(i_entry,hit);
    BOOST_CHECK(load(vector_special_hit_ds,sphit_vec));

    auto c = Cluster(i_entry, all_hits.at(i_entry));
    BOOST_CHECK(load(cluster_ds,c));

    std::vector<Cluster> clusters;
    clusters.emplace_back(2, all_hits.at(0));
    clusters.emplace_back(3, all_hits.at(1));
    BOOST_CHECK(load(vector_cluster_ds,clusters));

    std::map<int,Cluster> map_clusters = {
      { 2, Cluster(2, all_hits.at(0)) },
      { 3, Cluster(3, all_hits.at(1)) }
    }, read_map;
    BOOST_CHECK_NO_THROW(map_cluster_ds.load());
    BOOST_CHECK_NO_THROW(read_map = map_cluster_ds.get());
    for (auto const& [key,val] : map_clusters) {
      auto mit{read_map.find(key)};
      BOOST_CHECK(mit != read_map.end());
      BOOST_CHECK(mit->second == val);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
