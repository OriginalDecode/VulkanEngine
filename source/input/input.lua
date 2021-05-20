project "Input"
    kind "StaticLib"
    location(".")
    files{"**.cpp", "**.h", "**.hpp", "**.c"}
    dependson{"Core"}
    links { "Core" }