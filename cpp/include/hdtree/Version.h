/**
 * @file Version.h
 */

#pragma once

#include <string>

namespace hdtree {

/**
 * the recorded CMake version of fire
 *
 * This is defined by the VERSION keyword
 * in the 'project' command in CMakeLists.txt.
 */
const std::string& VERSION();

/**
 * The name of this API for HDTree
 */
const std::string& API();

}  // namespace hdtree
