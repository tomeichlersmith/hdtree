#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "HDTree" for configuration "Debug"
set_property(TARGET HDTree APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(HDTree PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libHDTree.so"
  IMPORTED_SONAME_DEBUG "libHDTree.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS HDTree )
list(APPEND _IMPORT_CHECK_FILES_FOR_HDTree "${_IMPORT_PREFIX}/lib/libHDTree.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
