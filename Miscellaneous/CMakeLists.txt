# add_executable(Miscellaneous non_null_smart_ptr.cpp)
add_executable(Miscellaneous pager_test.cpp)

target_include_directories(Miscellaneous PRIVATE
    ${CMAKE_SOURCE_DIR}/Miscellaneous
)

target_link_libraries(Miscellaneous
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(Miscellaneous)
