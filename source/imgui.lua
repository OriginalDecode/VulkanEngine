project "ImGui"
    kind "StaticLib"
    location("./external_libs/imgui/")
    includedirs { "./external_libs" ,"$(VULKAN_SDK)/Include/" }
    files{  "external_libs/imgui/*.h", 
            "external_libs/imgui/*.cpp", 
            "external_libs/imgui/misc/**.cpp", 
            "external_libs/imgui/misc/**.h",
            "external_libs/imgui/examples/imgui_impl_win32.*", 
            "external_libs/imgui/examples/imgui_impl_vulkan.*" }
    if _OPTIONS["cflags"] ~= "freetype" then
        print("excluding freetype")
        excludes { "external_libs/imgui/misc/freetype/**" }
    end