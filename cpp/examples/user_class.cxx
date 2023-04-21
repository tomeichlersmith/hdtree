/**
 * @file user_classes.cxx
 * Example of saving and loading user-defined C++ classes 
 */

// for generating random data
#include <random>

// for interacting with HDTrees
#include "hdtree/Tree.h"

// utility functions for example programs
#include "examples.h"

/**
 * Example user class
 */
class MyData {
  float x_, y_, z_;
  // grant hdtree access so we can keep the `attach` method private
  friend class hdtree::access;
  // this is where the name of data on disk is assigned to the
  // variable name of data in memory
  template <typename Branch>
  void attach(Branch& b) {
    b.attach("x", x_);
    b.attach("y", y_);
    b.attach("z", z_);
  }
 public:
  MyData() = default;
  MyData(float x, float y, float z)
    : x_{x}, y_{y}, z_{z} {}
  // HDTree also requires classes to have a `clear` method
  // for resetting the instance to a "non-assigned" state
  void clear() {
    x_ = 0.;
    y_ = 0.;
    z_ = 0.;
  }
  // helper function since we know what this data means
  float mag() const {
    return sqrt(x_*x_+y_*y_+z_*z_);
  }
};

int main(int argc, char** argv) try {
  /**
   * parse command line for arguments
   */
  std::string filename, treename;
  int rc = hdtree::examples::parse_single_file_args(argc, argv, filename, treename);
  if (rc != 0) return rc;

  { // write a simple file with some random data points
    auto tree = hdtree::Tree::save(filename, treename);
    /**
     * Once the MyData::attach method is written, it can be put
     * into STL containers (or as a member of other user classes)
     * like any other serializable class
     */
    auto& my_data = tree.branch<std::vector<MyData>>("my_data");
    // initialization of random number generator
    std::mt19937 rng;  // no argument -> no seed
    std::uniform_real_distribution<double> norm(0., 1.);
    std::uniform_int_distribution<std::size_t> uniform(1, 100);

    for (std::size_t i{0}; i < 100; ++i) {
      std::size_t size = uniform(rng);
      for (std::size_t j{0}; j < size; ++j) {
        my_data->emplace_back(norm(rng), norm(rng), norm(rng));
      }

      tree.save();
    }

    // final flushing accomplished when tree and its branches
    // go out of scope and are destructed
  }

  { // load back from same file and write the average mag as a new branch
    auto tree = hdtree::Tree::inplace(filename, treename);
    auto& my_data = tree.get<std::vector<MyData>>("my_data");
    auto& avg_mag = tree.branch<float>("avg_mag");
    tree.for_each([&]() {
        if (my_data->size() > 0) {
          float tot_mag = 0.;
          for (const MyData& d : *my_data) {
            tot_mag += d.mag();
          }
          *avg_mag = tot_mag/my_data->size();
        } else {
          *avg_mag = -1;
        }
    });

    // final flushing accomplished when tree and its branches
    // go out of scope and are destructed
  }

  return 0;
} catch (const hdtree::HDTreeException& e) {
  std::cerr << "ERROR " << e << std::endl;
  return 1;
}
