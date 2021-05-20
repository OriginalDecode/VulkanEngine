startproject "UnitTest"
project "UnitTest" --project name
    targetname "%{wks.name}_%{cfg.buildcfg}Test"
    kind "ConsoleApp" --type [ConsoleApp, WindowedApp, SharedLib, StaticLib, Makefile, Utility, None, AndroidProj], WindowedApp is important on Windows and Mac OS X
    location (".")
    defines { "UNIT_TEST" }
    dependson { "Core" }
    links { "Core", 
            "Input", 
            "Logger", 
            "../external_libs/googletest/lib/Debug/gtestd.lib", 
            "../external_libs/googletest/lib/Debug/gtest_maind.lib",
            "../external_libs/googletest/lib/Debug/gmockd.lib", 
            "../external_libs/googletest/lib/Debug/gmock_maind.lib" } --libraries to link
    files { "*.cpp" }