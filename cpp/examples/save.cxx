/**
 * @file save.cxx
 * Example of saving a new HDTree into a file
 */

// for generating random data
#include <random>

// for interacting with HDTrees
#include "hdtree/Tree.h"

// utility functions for example programs
#include "examples.h"

int main(int argc, char** argv) try {
  /**
   * parse command line for arguments
   */
  std::string filename, treename;
  int rc = hdtree::examples::parse_single_file_args(argc, argv, filename, treename);
  if (rc != 0) return rc;

  /**
   * Create a tree by defining what file it is in
   * and where it resides within that file
   */
  auto tree = hdtree::Tree::save(filename, treename);

  /**
   * Create branches to define what type of information will
   * go into the HDTree. The hdtree::Tree::branch function
   * returns a handle to the created hdtree::Branch object.
   * This object can (and should) be used to interace with
   * the values that will be stored in the HDTree on disk
   * in order to reduce the number of in-memory copies that
   * need to happen. Here, we use `auto&` to avoid typing
   * out all the C++ template nonsense that hdtree::Branch
   * does under-the-hood.
   *
   * Each branch handle can be treated as a pointer
   * to the underlying type.
   *
   * **Note**: Branch handles are invalid after the tree they
   * were created from is deleted.
   */
  auto& i_entry = tree.branch<std::size_t>("i_entry");
  auto& rand_nums = tree.branch<std::vector<double>>("rand_nums");

  /**
   * Initialization of random number generation.
   * Not really applicable to HDTree, just used here to
   * show that varying length vectors can be serialized
   * with ease
   */
  std::mt19937 rng;  // no argument -> no seed
  std::uniform_real_distribution<double> norm(0., 1.);
  std::uniform_int_distribution<std::size_t> uniform(1, 100);

  /**
   * Actual update and filling of the HDTree.
   *
   * You can see here how we can treat `i_entry` 
   * as if it was a properly initialized `std::size_t *` 
   * and `rand_nums` * as if it was a properly 
   * initialized `std::vector<double> *`.
   */
  for (std::size_t i{0}; i < 100; ++i) {
    *i_entry = i;
    std::size_t size = uniform(rng);
    for (std::size_t j{0}; j < size; j++) {
      rand_nums->push_back(norm(rng));
    }

    /**
     * We choose to save each value of the loop into the tree.
     */
    tree.save();
  }

  /**
   * The final flushing of the data to disk as well as handle
   * cleanup procedures will all be handled automatically by
   * deconstruction.
   */
  return 0;
} catch (const hdtree::HDTreeException& e) {
  std::cerr << "ERROR " << e << std::endl;
  return 1;
}
