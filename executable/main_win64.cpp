#include <Windows.h>
#include <iostream>

#include <graphics/Window.h>
#include <graphics/GraphicsEngine.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
bool gameRunning = true; //we'll need this in the WindowProc

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR args, int)
{
    Window window{ { 1920, 1080, instance, WindowProc } };

    Graphics::GraphicsEngine* graphics_engine = Graphics::GraphicsEngine::Create();
	graphics_engine->Init(window);
    
    MSG msg;
	do
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT || msg.message == WM_CLOSE)
		{
			gameRunning  = false;
			break;
		}

        graphics_engine->Present();

    } while ( gameRunning );
 
    

 
    return 0;
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
		case WM_CREATE:
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_SIZE:
			break;
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
			{
			}
			else
			{
			}
		}break;
		case WM_ENTERSIZEMOVE:
			break;
		case WM_EXITSIZEMOVE:
			break;
		case WM_CLOSE:
			gameRunning = false;
			break;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_SYSKEYDOWN:
			if (wParam == VK_RETURN)
			{
				if ((HIWORD(lParam) & KF_ALTDOWN))
				{

				}
			}
			break;
		case WM_INPUT:
		{
		} break;
		default:
			break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}