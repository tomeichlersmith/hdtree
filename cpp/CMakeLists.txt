# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(HDTree
  VERSION 0.1.0
  DESCRIPTION "HDTree C++ API"
  LANGUAGES C CXX)

# sanitize on Debug builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined")

# Set the default release type to "Debug"
# If CMAKE_BUILD_TYPE is set at the command line, it's respected.
set(default_build_type "Debug")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
endif()

execute_process(
  COMMAND git describe --tags
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  RESULT_VARIABLE git_rc
  OUTPUT_VARIABLE git_describe)
if (NOT ${git_rc} EQUAL 0)
  if (CMAKE_BUILD_TYPE EQUAL "Debug")
    message(WARNING "System cannot deduce git tag to check HDTree version number")
  endif()
else()
  string(REGEX REPLACE "^v" "" git_describe ${git_describe})
  string(REGEX REPLACE "\n" "" git_describe ${git_describe})
  string(REGEX REPLACE "-.*$" "" git_describe ${git_describe})
  set(git_vers ${git_describe})
  if (NOT ${git_vers} EQUAL ${PROJECT_VERSION})
    message(FATAL_ERROR "Deduced git verison ${git_vers} is not equal to CMake version ${PROJECT_VERSION}")
  endif()
endif()

message(STATUS "Building HDTree C++ API ${PROJECT_VERSION} ${CMAKE_BUILD_TYPE}")

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# necessary dependencies
find_package(HighFive REQUIRED)

# writes the CMake project version into package
configure_file(${PROJECT_SOURCE_DIR}/src/Version.cxx.in
              ${PROJECT_BINARY_DIR}/src/Version.cxx)

add_library(HDTree SHARED
  src/Atomic.cxx
  src/Reader.cxx
  src/Writer.cxx
  ${PROJECT_BINARY_DIR}/src/Version.cxx)
target_include_directories(HDTree PUBLIC 
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>"
  )
target_link_libraries(HDTree PUBLIC HighFive)

# Compiling the HDTree library requires features introduced by the cxx 17 standard.
set_target_properties(HDTree
  PROPERTIES CXX_STANDARD 17
             CXX_STANDARD_REQUIRED YES
             CXX_EXTENSIONS NO)

# installing the HDTree target and generate CMake-compatible package files
#   is done by following the documentation linked below
# https://cmake.org/cmake/help/git-stage/guide/importing-exporting/index.html
install(TARGETS HDTree
  EXPORT HDTreeTargets 
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
  )
# need pattern to exclude version header configured by CMake above
install(DIRECTORY include/ DESTINATION include/ FILES_MATCHING PATTERN "*.h")

# install the export file
install(EXPORT HDTreeTargets
  FILE HDTreeTargets.cmake
  DESTINATION lib/cmake/HDTree)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/HDTreeConfig.cmake"
      INSTALL_DESTINATION lib/cmake/HDTree
      )
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/HDTreeConfigVersion.cmake"
  COMPATIBILITY AnyNewerVersion)
install(FILES
      "${CMAKE_CURRENT_BINARY_DIR}/HDTreeConfig.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/HDTreeConfigVersion.cmake"
      DESTINATION lib/cmake/HDTree)

include(CTest)
if (BUILD_TESTING)
  add_subdirectory(test)
  #  add_subdirectory(examples)
endif()
