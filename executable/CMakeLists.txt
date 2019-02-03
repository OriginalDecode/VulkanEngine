
add_executable(engine WIN32 main_win64.cpp)

set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)
# set_target_properties(engine PROPERTIES OUTPUT_NAME )
# file(GLOB source "*.cpp")

include_directories(${CMAKE_SOURCE_DIR}) #include files from application

target_link_libraries(engine application)
target_link_libraries(engine graphics)

install(TARGETS engine RUNTIME DESTINATION bin)

