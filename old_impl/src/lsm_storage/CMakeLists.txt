# Set the source files for the LsmStorage component
file(GLOB SERVER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create the library target for the LsmStorage component
add_library(LsmStorage ${SERVER_SOURCES})

# Include directories for the LsmStorage component
target_include_directories(LsmStorage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(LsmStorage PUBLIC LSMIterators MemTable)