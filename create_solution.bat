@echo off

rmdir /S /Q build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=.. -DBUID_SHARED_LIBS=ON -DCMAKE_GENERATOR_PLATFORM=x64 -G "Visual Studio 14 2015"

cd ..