project "Graphics"
    kind "StaticLib"
    location (".")    
    files { "**.cpp", 
            "**.c", 
            "**.cxx", 
            "**.h", 
            "**.hpp" }

    includedirs { "../external_libs/" }
    dependson { "Core", "ImGui" }
    links { "$(VULKAN_SDK)/lib/vulkan-1.lib", 
            "../thirdparty/freetype/freetype.lib", 
            "ImGui" }

    includedirs { "$(VULKAN_SDK)/Include/",
                  "../thirdparty/freetype/" }
    if _OPTIONS["project"] == "unit_test" then
        flags {"ExcludeFromBuild"}
    end