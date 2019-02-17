@echo off


set CONFIG=%1
set TYPE=%2
set BUILD=%3


IF "%CONFIG%"=="configure" (
    IF NOT DEFINED TYPE (
        echo No configuarion set.
    ) else (
        premake5 %TYPE%
    )
) else (
    IF NOT DEFINED BUILD (
        echo No build parameters
    ) else (
        IF EXIST "build" (
            cd "build/Engine.sln"

        ) else (
            echo No solution found. Failed to build
        )
        REM "/property:GenerateFullPaths=true",
                REM "/t:build"
    )
    
    
)