find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(hdtree
  main_test.cxx
  branch.cxx
  )
target_link_libraries(hdtree PRIVATE Boost::unit_test_framework HDTree) 

add_custom_target(test
  COMMAND $<TARGET_FILE:hdtree>
  DEPENDS HDTree
)
