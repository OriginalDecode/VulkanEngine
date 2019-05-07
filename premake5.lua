
-- local fnc = require "test"

newoption {
    trigger = "platform",
    value = "Platform",
    description = "Set platform",
    allowed = {
        { "windows", "Windows" },
        { "linux", "Linux" }
    }
}

newoption {
    trigger = "project",
    value = "Project",
    description = "Set project flag",
    allowed = {
        { "unit_test", "unit test" },
        { "engine", "engine" }
    }
}

--beware of the scope issue https://github.com/premake/premake-core/wiki/Scopes-and-Inheritance
if _OPTIONS["project"] == nil then
    print("No project set, will not configure")
    return 
end 

if _OPTIONS["project"] == "engine" then
    print("configuring Engine")
workspace "Engine" --this is the solution name in a vs project if no filename specified
elseif _OPTIONS["project"] == "unit_test" then
    print("configuring UnitTest")
workspace "UnitTest"
else
return
end
    -- fnc.setWorkspace("Engine")
    -- fnc.addConfig("Debug")
    -- fnc.addConfig("Release")
    -- fnc.setPlatform("Windows")
    configurations { "Debug" , "Release" }
    platforms { _OPTIONS["platform"] }
    debugdir "%{wks.location}../bin"
    -- language "C++"
    cppdialect "C++14"
    disablewarnings { "4201" }
    -- flags { "C++14" }
--filename "whiteroom"
    -- platforms { "Windows" }
    architecture "x64"
    includedirs { ".\\" }
    -- libdirs { "" }
    flags { "FatalWarnings" }
    warnings "Extra"

    objdir "%{wks.location}/obj/%{cfg.buildcfg}/%{prj.name}"

    filter "platforms:Windows"
        defines { "_WIN32", "_CRT_SECURE_NO_WARNINGS" }
            
    filter "platforms:Linux"
        defines { "_GCC_" }
    
    filter "kind:StaticLib"
        targetdir "%{wks.location}/lib/%{cfg.buildcfg}"

    filter "kind:WindowedApp"
        targetdir "%{wks.location}/../bin"


    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines {"NDEBUG"}
        optimize "On"

    if _OPTIONS["project"] ~= nil then
        if _OPTIONS["project"] == "engine" then
            project "Executable" --project name
                targetname "%{wks.name}_%{cfg.buildcfg}"
                kind "WindowedApp" --type [ConsoleApp, WindowedApp, SharedLib, StaticLib, Makefile, Utility, None, AndroidProj], WindowedApp is important on Windows and Mac OS X
                location ("./executable")

                dependson { "Core", "Graphics" }
                links { "Graphics", "Core" } --libraries to link

                files { "executable/*.cpp" }
        else
            project "UnitTest" --project name
                targetname "%{wks.name}_%{cfg.buildcfg}"
                kind "ConsoleApp" --type [ConsoleApp, WindowedApp, SharedLib, StaticLib, Makefile, Utility, None, AndroidProj], WindowedApp is important on Windows and Mac OS X
                location ("./unit_test")

                dependson { "Core", "Graphics" }
                links { "Graphics", "Core" } --libraries to link

                files { "unit_test/*.cpp" }
        end
    else
        print("No project set")
    end

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

    