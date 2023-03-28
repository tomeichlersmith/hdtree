#pragma once

#include <utility>

/**
 * namespace holding utility functions for example programs
 *
 * These functions are not supposed to be important to the logic
 * of the HDTree C++ API
 */
namespace hdtree::examples {

/**
 * print help for the basic command line parser
 *
 * @param[in] program name of program that is being run
 */
void print_help(const std::string& program) {
  std::cout 
    << "USAGE:\n"
    << "  " << program << " [-h | --help ] FILE TREE\n"
    << "\n"
    << "OPTIONS:\n"
    << "  -h, --help : print this help and exit\n"
    << "\n"
    << "ARGUMENTS:\n"
    << "  FILE : file to access for interacting with HDTree\n"
    << "  TREE : name of tree in that file to interact with\n"
    << std::endl;
}

/**
 * basic command line parser
 *
 * @param[in] argc arg count from main
 * @param[in] argv arg array from main
 * @param[out] filename deduced output file name
 * @param[out] treename deduced output tree name
 * @return returne code, continue processing if zero, exit main with this value otherwise
 */
int parse_args(int argc, char** argv, std::string& filename, std::string& treename) {
  std::string program{argv[0]};
  std::vector<std::string> positionals;
  for (int i_arg{1}; i_arg < argc; ++i_arg) {
    std::string arg{argv[i_arg]};
    if (arg == "-h" or arg == "--help") {
      print_help(program);
      return 1;
    } else {
      positionals.emplace_back(arg);
    }
  }

  if (positionals.size() == 0) {
    // absolutely zero arguments => assume user wants help
    print_help(program);
    return 2;
  }

  if (positionals.size() != 2) {
    std::cerr << " ERROR: " << program << " requires two arguments: FILE TREE" << std::endl;
    return -1;
  }

  filename = positionals.at(0);
  treename = positionals.at(1);
  return 0;
}

}
