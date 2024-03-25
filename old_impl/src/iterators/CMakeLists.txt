# Set the source files for the LSMIterators component
file(GLOB SERVER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create the library target for the LSMIterators component
add_library(LSMIterators ${SERVER_SOURCES})

# Include directories for the LSMIterators component
target_include_directories(LSMIterators PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(LSMIterators PUBLIC MemTable)