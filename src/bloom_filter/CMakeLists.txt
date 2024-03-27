# Set the source files for the BloomFilter component
file(GLOB SERVER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create the library target for the BloomFilter component
add_library(BloomFilter ${SERVER_SOURCES})

# Include directories for the BloomFilter component
target_include_directories(BloomFilter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(BloomFilter PUBLIC)