#include <iostream>
#include <application/Application.h>

int main()
{
    Application app;
    app.Update();

    printf("Hello, World!\n");

    std::getchar();
    return 0;
}