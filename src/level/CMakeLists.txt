# Set the source files for the Level_SSTables component
file(GLOB SERVER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create the library target for the Level_SSTables component
add_library(Level_SSTables ${SERVER_SOURCES})

# Include directories for the Level_SSTables component
target_include_directories(Level_SSTables PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(Level_SSTables PUBLIC BloomFilter)
target_compile_options(Level_SSTables PRIVATE -g)