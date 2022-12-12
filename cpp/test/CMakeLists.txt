find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(test_hdtree
  main_test.cxx
  branch.cxx
  )
target_link_libraries(test_hdtree PRIVATE Boost::unit_test_framework HDTree) 

add_test(NAME "TestHDTree"
  COMMAND $<TARGET_FILE:test_hdtree> --report_level=detailed
  WORKING_DIRECTORY test
)
