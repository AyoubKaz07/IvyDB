find_package(GTest REQUIRED)

add_executable(lsm_tree_test lsm_tree_test.cpp)

target_link_libraries(lsm_tree_test
    PRIVATE
        GTest::GTest
        GTest::Main
        LSMTree
)

add_test(lsm_tree_gtests lsm_tree_test)
# Enable debug mode
target_compile_options(lsm_tree_test PRIVATE -g)
