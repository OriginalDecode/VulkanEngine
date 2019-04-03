
-- local fnc = require "test"
require "lua"

--beware of the scope issue https://github.com/premake/premake-core/wiki/Scopes-and-Inheritance
workspace "Engine" --this is the solution name in a vs project if no filename specified

    -- fnc.setWorkspace("Engine")
    -- fnc.addConfig("Debug")
    -- fnc.addConfig("Release")
    -- fnc.setPlatform("Windows")
    platforms { "Windows", "Linux" }
    configurations { "Debug" , "Release" }
    debugdir "%{wks.location}/bin/Resources"
    -- language "C++"
    cppdialect "C++14"
    -- flags { "C++14" }
--filename "whiteroom"
    -- platforms { "Windows" }
    architecture "x64"
    includedirs { ".\\" }
    -- libdirs { "" }
    flags { "FatalCompileWarnings" }
    warnings "Extra"
    location "build"
    
    
    objdir "%{wks.location}/obj/%{cfg.buildcfg}/%{prj.name}"

    filter "kind:StaticLib"
        targetdir "%{wks.location}/lib/%{cfg.buildcfg}"

    filter "kind:WindowedApp"
        targetdir "%{wks.location}/bin/%{cfg.buildcfg}"


    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines {"NDEBUG"}
        optimize "On"

    
    project "Executable" --project name
        targetname("engine")
        kind "WindowedApp" --type [ConsoleApp, WindowedApp, SharedLib, StaticLib, Makefile, Utility, None, AndroidProj], WindowedApp is important on Windows and Mac OS X
        location ("./executable")
        
        dependson { "Core", "Graphics" }
        links { "Graphics", "Core" } --libraries to link
        
        files { "executable/*.cpp" }

        

    project "Graphics"
        kind "StaticLib"
        location ("./graphics")    
        files { "graphics/*.cpp", "graphics/*.h" }
        dependson { "Core" }
        links { "$(VULKAN_SDK)/lib/vulkan-1.lib" }
        includedirs { "$(VULKAN_SDK)/Include/" }
        -- symbolspath does not seem to work as inteded
        -- symbolspath "%{wks.location}/bin/%{cfg.buildcfg}/%{prj.name}.pdb"
        

    project "Core"
        kind "StaticLib"
        location ("./core")

        files { "core/**.cpp", "core/**.h" }

    workspace "*"
        filter "platforms:Windows"
            defines { "_WIN32", "_CRT_SECURE_NO_WARNINGS" }
            
        filter "platforms:Linux"
            defines { "_GCC_" }