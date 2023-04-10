
add_executable(hdtree-eg-save save.cxx)
target_link_libraries(hdtree-eg-save PRIVATE HDTree)

add_executable(hdtree-eg-transform transform.cxx)
target_link_libraries(hdtree-eg-transform PRIVATE HDTree)

add_executable(hdtree-eg-load load.cxx)
target_link_libraries(hdtree-eg-load PRIVATE HDTree)
