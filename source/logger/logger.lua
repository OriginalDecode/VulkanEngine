project "Logger"
    kind "StaticLib"
    location(".")
    links { "Core" }
    dependson { "Core" }
    files{"**.cpp", "**.h", "**.hpp", "**.c"}
