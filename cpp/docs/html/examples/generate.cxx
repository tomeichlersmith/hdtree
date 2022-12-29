/**
 * @file generate.cxx
 * Example generation of a HDTree
 */

#include <random>

#include "hdtree/Tree.h"

int main() {
  auto tree = hdtree::Tree::save("generate.h5", "example");

  auto& i_entry = tree.branch<std::size_t>("i_entry");
  auto& rand_nums = tree.branch<std::vector<double>>("rand_nums");

  std::mt19937 rng;  // no argument -> no seed
  std::uniform_real_distribution<double> norm(0., 1.);
  std::uniform_int_distribution<std::size_t> uniform(1, 100);

  for (std::size_t i{0}; i < 1'000'000; i++) {
    i_entry = i;
    std::size_t size = uniform(rng);
    for (std::size_t j{0}; j < size; j++) {
      rand_nums->push_back(norm(rng));
    }

    tree.save();
  }
}
