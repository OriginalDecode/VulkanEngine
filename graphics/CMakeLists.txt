file(GLOB sources *.cpp *.h)

add_library(graphics ${sources})
set_target_properties(graphics PROPERTIES LINKER_LANGUAGE CXX)
include_directories(${CMAKE_SOURCE_DIR}/commonlib) #this directory is my current folder and not the root. Making it include from that point and down not up and down.

install(TARGETS graphics 
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib )
