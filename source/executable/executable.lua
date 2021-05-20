startproject "Executable"
project "Executable" --project name
    targetname "%{wks.name}_%{cfg.buildcfg}"
    kind "WindowedApp" --type [ConsoleApp, WindowedApp, SharedLib, StaticLib, Makefile, Utility, None, AndroidProj], WindowedApp is important on Windows and Mac OS X
    location (".")
    includedirs { "../external_libs/" }
    dependson { "Core", "Graphics", "Input", "Logger", "Game" }
    links { "Graphics", "Core", "Input", "Logger", "Game" } --libraries to link
    files { "*.cpp" }