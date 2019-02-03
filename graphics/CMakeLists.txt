file(GLOB sources *.cpp *.h)

add_library(graphics ${sources})
set_target_properties(graphics PROPERTIES LINKER_LANGUAGE CXX)
include_directories(${CMAKE_SOURCE_DIR})

install(TARGETS graphics 
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib )
