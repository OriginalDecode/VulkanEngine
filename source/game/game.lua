project "Game"
    kind "StaticLib"
    location(".")
    files{"**.cpp", "**.h", "**.hpp", "**.c"}
    dependson{"Core", "Input"}
    links { "Core", "Input" }