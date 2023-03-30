/**
 * @file examples.h
 * Header file including helper utility functions unrelated to HDTree
 * and just helpful for command line programs.
 */

/**
 * @dir examples
 * Example programs that are continuously compiled along with developments.
 */
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
 * print help for the single file command line parser
 *
 * @param[in] program name of program that is being run
 * @param[in] two_files true if searching for two files
 */
void print_help(const std::string& program, bool two_files) {
  std::cout 
    << "USAGE:\n"
    << "  " << program << " [-h | --help ] "
    << (two_files ? "SRC_FILE SRC_TREE [DEST_FILE DEST_TREE]" : "FILE TREE") << "\n"
    << "\n"
    << "OPTIONS:\n"
    << "  -h, --help : print this help and exit\n"
    << "\n"
    << "ARGUMENTS:\n";
  if (two_files) {
    std::cout
      << "  SRC_FILE  : source file from-which to read HDTree (and write if no DEST_FILE)\n"
      << "  SRC_TREE  : source tree from-which to read (and write if no DEST_TREE)\n"
      << "  DEST_FILE : destination file to write to (optional but needs DEST_TREE as well)\n"
      << "  DEST_TREE : destination tree to write to (optional but needs DEST_FILE as well)\n";
  } else {
    std::cout
      << "  FILE : file to access for interacting with HDTree\n"
      << "  TREE : name of tree in that file to interact with\n";
  }
  std::cout << std::endl;
}

/**
 * single file command line parser
 *
 * @param[in] argc arg count from main
 * @param[in] argv arg array from main
 * @param[out] filename deduced output file name
 * @param[out] treename deduced output tree name
 * @return return code, continue processing if zero, exit main with this value otherwise
 */
int parse_single_file_args(int argc, char** argv, 
    std::string& filename, std::string& treename) {
  std::string program{argv[0]};
  std::vector<std::string> positionals;
  for (int i_arg{1}; i_arg < argc; ++i_arg) {
    std::string arg{argv[i_arg]};
    if (arg == "-h" or arg == "--help") {
      print_help(program,false);
      return 1;
    } else {
      positionals.emplace_back(arg);
    }
  }

  if (positionals.size() == 0) {
    // absolutely zero arguments => assume user wants help
    print_help(program,false);
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

/**
 * potentially two file command line parser
 *
 * @param[in] argc arg count from main
 * @param[in] argv arg array from main
 * @param[out] src deduced source file and tree names
 * @param[out] dest deduced destination file and tree names
 * @return return code, continue processing if zero, exit main with this value otherwise
 */
int parse_two_file_args(int argc, char** argv, 
    std::pair<std::string,std::string>& src, std::pair<std::string,std::string>& dest) {
  std::string program{argv[0]};
  std::vector<std::string> positionals;
  for (int i_arg{1}; i_arg < argc; ++i_arg) {
    std::string arg{argv[i_arg]};
    if (arg == "-h" or arg == "--help") {
      print_help(program,true);
      return 1;
    } else {
      positionals.emplace_back(arg);
    }
  }

  if (positionals.size() == 0) {
    // absolutely zero arguments => assume user wants help
    print_help(program,true);
    return 2;
  }

  if (positionals.size() != 2 or positionals.size() != 4) {
    std::cerr << " ERROR: " << program << " requires two or four arguments." << std::endl;
    return -1;
  }

  src.first = positionals.at(0);
  src.second = positionals.at(1);
  if (positionals.size() > 2) {
    dest.first = positionals.at(2);
    dest.second = positionals.at(3);
  }
  return 0;
}

}
