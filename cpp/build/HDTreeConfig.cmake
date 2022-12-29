
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was Config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

# leave early if we have already found fire
if (HDTree_FOUND)
  return()
endif()

message(STATUS "HDTree 0.4.5 found")

# We need to find_dependency for the same packages
#   we needed to find_package for
#   this registers the packages we need to dynamically
#   link with other projects using fire
include(CMakeFindDependencyMacro)
find_dependency(HighFive)

include("${CMAKE_CURRENT_LIST_DIR}/HDTreeTargets.cmake")

check_required_components(HDTree)
