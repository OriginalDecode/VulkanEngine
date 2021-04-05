#include "WindowsMain.h"
#include "imgui/examples/imgui_impl_win32.h"
#include "graphics/Window.h"

#include <Windows.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) /* very windows
																					  specific */
{

	if(ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch(message)
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
			if(LOWORD(wParam) == WA_INACTIVE)
			{
			}
			else
			{
			}
		}
		break;
		case WM_ENTERSIZEMOVE:
			break;
		case WM_EXITSIZEMOVE:
			break;
		case WM_CLOSE:
			break;
		case WM_SYSCOMMAND:
			if((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return false;
			break;
		case WM_SYSKEYDOWN:
			if(wParam == VK_RETURN)
			{
				if((HIWORD(lParam) & KF_ALTDOWN))
				{
				}
			}
			break;
		case WM_INPUT:
		{
		}
		break;
		default:
			break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

WindowsMain::~WindowsMain() {}

void WindowsMain::Init(HINSTANCE instanceHandle)
{
	Window::CreateInfo createInfo{ 1920.f, 1080.f, instanceHandle, WindowProc };
	m_Window = new Window(createInfo);
	ImGui_ImplWin32_Init(m_Window->GetHandle());
}

bool WindowsMain::Update()
{
	MSG msg;
	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) // message pump
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if((msg.message & (WM_QUIT | WM_CLOSE)) != 0)
	{
		return false;
	}

	return true;
}
