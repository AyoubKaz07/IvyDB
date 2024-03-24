# Set the source files for the MemTable component
file(GLOB SERVER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create the library target for the MemTable component
add_library(MemTable ${SERVER_SOURCES})

# Include directories for the MemTable component
target_include_directories(MemTable PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)