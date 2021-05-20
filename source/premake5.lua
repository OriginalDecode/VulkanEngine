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
        { "engine", "engine" },
        { "thirdparty", "thirdparty" }
    }
}

newoption {
    trigger = "cflags",
    value = "cflags",
    description = "compiler flags to add things for compilation",
    allowed = {
        { "freetype", "freetype" }
    }
}

if os.isdir('../bin/') == nil then
    print('no directory, creating')
    os.mkdir('../bin/')
end

if os.isfile('../bin/.gitignore') == false then
    print('no')
    io.writefile('../bin/.gitignore', '*')
end
    

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
    configurations { "Debug" , "Release" }

    platforms { _OPTIONS["platform"] }
    debugdir "%{wks.location}../bin"
    
    cppdialect "c++17"
    disablewarnings { "4201" }
    architecture "x64"
    includedirs { ".\\" }
    warnings "Extra"

    objdir "%{wks.location}/obj/%{cfg.buildcfg}/%{prj.name}"
    staticruntime "on"
    filter "system:Windows"
        systemversion "latest"

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
        include("./executable/executable.lua")
    elseif _OPTIONS["project"] == "unit_test" then
        include("./unit_test/unittest.lua")
    end
end

    include("./graphics/graphics.lua")
    include("./input/input.lua")
    include("./core/core.lua")
    include("./game/game.lua")
    include("./logger/logger.lua")
    include("./imgui.lua")