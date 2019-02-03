cmake_minimum_required( VERSION 3.5.1 )
project (AriEngine C CXX)

# Version of the Software
set (AriEngine_VERSION_MAJOR 0)
set (AriEngine_VERSION_MINOR 1)



include(CheckCXXCompilerFlag)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_BINARY_DIR "build")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../bin/") #I am not a fan of specifying the output like this.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "../../lib/") #these does not work as I have intended them to do right now. Big sad
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../../lib/")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory("executable")
add_subdirectory("application")
# add_subdirectory("audio")
# add_subdirectory("commonlib")
# add_subdirectory("gamesystems")
# add_subdirectory("graphics")
# add_subdirectory("physics")

