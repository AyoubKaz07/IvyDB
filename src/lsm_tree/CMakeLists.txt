# Set the source files for the LSMTree component
file(GLOB SERVER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create the library target for the LSMTree component
add_library(LSMTree ${SERVER_SOURCES})

# Include directories for the LSMTree component
target_include_directories(LSMTree PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(LSMTree PUBLIC Level_SSTables Wal)
target_compile_options(LSMTree PRIVATE -g)