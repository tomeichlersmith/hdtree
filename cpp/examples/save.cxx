/**
 * @file generate.cxx
 * Example generation of a HDTree
 */

#include <random>

#include "hdtree/Tree.h"

int main() {
  /**
   * Create a tree by defining what file it is in
   * and where it resides within that file
   */
  auto tree = hdtree::Tree::save("generate.h5", "example");

  auto& i_entry = tree.branch<std::size_t>("i_entry");
  auto& rand_nums = tree.branch<std::vector<double>>("rand_nums");

  std::mt19937 rng;  // no argument -> no seed
  std::uniform_real_distribution<double> norm(0., 1.);
  std::uniform_int_distribution<std::size_t> uniform(1, 100);

  for (*i_entry = 0; *i_entry < 1'000'000; ++(*i_entry)) {
    std::size_t size = uniform(rng);
    for (std::size_t j{0}; j < size; j++) {
      rand_nums->push_back(norm(rng));
    }

    tree.save();
  }
}
