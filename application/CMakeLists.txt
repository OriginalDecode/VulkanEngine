# file(GLOB src /*.cpp)

add_library(application Application.cpp Application.h)
set_target_properties(application PROPERTIES LINKER_LANGUAGE CXX)

install(TARGETS application 
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib )
