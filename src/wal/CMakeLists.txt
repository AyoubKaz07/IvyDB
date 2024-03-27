# Set the source files for the Wal component
file(GLOB SERVER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create the library target for the Wal component
add_library(Wal ${SERVER_SOURCES})

# Include directories for the Wal component
target_include_directories(Wal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(Wal PUBLIC)
target_compile_options(Wal PRIVATE -g)