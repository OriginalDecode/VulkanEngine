workspace "Engine" --this is the solution name in a vs project if no filename specified
    --filename "whiteroom"
    configurations { "Debug" , "Release" }
    platforms { "Windows" }
    architecture "x64"
    includedirs { ".\\" }
    libdirs { "" }
    flags { "FatalCompileWarnings" }
    warnings "Extra"
    location "build"


    filter "StaticLib"
        targetdir "lib/%{cfg.buildcfg}"
    

    
    project "Executable" --project name
    kind "WindowedApp" --type [ConsoleApp, WindowedApp, SharedLib, StaticLib, Makefile, Utility, None, AndroidProj], WindowedApp is important on Windows and Mac OS X
    language "C++" --language
    links { "Graphics", "Core" } --libraries to link
    location ("./executable")
    targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
    objdir "%{wks.location}/obj/%{cfg.buildcfg}"

    dependson { "Core", "Graphics" }

    files { "executable/*.cpp" }

    filter "platforms:Windows"
        defines { "_WIN32" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines {"NDEBUG"}
        optimize "On"



project "Graphics"
    kind "StaticLib"
    language "C++"
    location ("./graphics")    
    files { "graphics/*.cpp", "graphics/*.h" }

    targetdir "%{wks.location}/lib/%{cfg.buildcfg}"
    objdir "%{wks.location}/obj/%{cfg.buildcfg}"
    
    
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines {"NDEBUG"}
        optimize "On"


project "Core"
    kind "StaticLib"
    language "C++"
    location ("./core")

    files { "core/**.cpp", "core/**.h" }

    targetdir "%{wks.location}/lib/%{cfg.buildcfg}"
    objdir "%{wks.location}/obj/%{cfg.buildcfg}"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines {"NDEBUG"}
        optimize "On"
