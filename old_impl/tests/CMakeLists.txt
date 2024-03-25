find_package(GTest REQUIRED)

add_executable(lsm_storage_test lsm_storage_test.cpp)

target_link_libraries(lsm_storage_test
    PRIVATE
        GTest::GTest
        LsmStorage
)

add_test(lsm_storage_gtests lsm_storage_test)