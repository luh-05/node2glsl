add_executable(blender_tests
  test_blender_ir.cpp
  test_blender_xml.cpp
)
target_link_libraries(blender_tests
  PRIVATE
    GTest::gtest_main
    msk::core
    msk::ir
    msk::blender
)
target_compile_definitions(blender_tests
  PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)

include(GoogleTest)
gtest_discover_tests(blender_tests)

