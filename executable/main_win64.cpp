

#include "graphics/Window.h"
#include "graphics/GraphicsEngine.h"
#include "core/Timer.h"
#include "input/InputManager.h"
#include "Logger/Debug.h"

#include "thirdparty/imgui/imgui.h"
#include "graphics/imgui_impl_win32.h"

#include <memory>
#include <iostream>
#include <Windows.h>



LRESULT CALLBACK WindowProc( HWND, UINT, WPARAM, LPARAM );
bool gameRunning = true; // we'll need this in the WindowProc

int WINAPI WinMain( HINSTANCE instance, HINSTANCE, LPSTR, int )
{
	// Window* window = new Window({ 1920, 1080, instance, WindowProc });


	Graphics::CreateImGuiContext();

	Log::Debug::Create();

	Window::CreateInfo createInfo{ 1920.f, 1080.f, instance, WindowProc };

	std::unique_ptr<Window> window = std::make_unique<Window>( createInfo );
	window->SetText( "Engine2" );

	Graphics::GraphicsEngine::Create();
	Graphics::GraphicsEngine& graphics_engine = Graphics::GraphicsEngine::Get();

	if( !graphics_engine.Init( *window ) )
		return 1;

	Input::InputManager& input = Input::InputManager::Get();
	input.Initialize( window->GetHandle(), instance );

	Core::Timer timer;
	timer.Start();

	ImGui_ImplWin32_Init( window->GetHandle() );

	MSG msg;
	do
	{
		timer.Update();

		char temp[100] = { 0 };
		sprintf_s(temp, "FPS : %.3f dt: %.3f", 1.f / timer.GetTime(), timer.GetTime());
		window->SetText( temp );
		
		/* Windows Specific */
		while( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) ) //message pump
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		if( msg.message == WM_QUIT || msg.message == WM_CLOSE )
		{
			gameRunning = false; //not so specific
			break;
		}
		/* Windows Specific */

		input.Update();
		graphics_engine.Present( timer.GetTime() );

	} while( gameRunning );

	Input::InputManager::Destroy();

	Log::Debug::Destroy();

	return 0;
}


extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) /* very windows specific */
{

	if( ImGui_ImplWin32_WndProcHandler( hWnd, message, wParam, lParam ) )
		return true;

	switch( message )
	{
		case WM_CREATE:
			break;
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;
		case WM_SIZE:
			break;
		case WM_ACTIVATE:
		{
			if( LOWORD( wParam ) == WA_INACTIVE )
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
			gameRunning = false;
			break;
		case WM_SYSCOMMAND:
			if( ( wParam & 0xfff0 ) == SC_KEYMENU ) // Disable ALT application menu
				return 0;
			break;
		case WM_SYSKEYDOWN:
			if( wParam == VK_RETURN )
			{
				if( ( HIWORD( lParam ) & KF_ALTDOWN ) )
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
	return DefWindowProc( hWnd, message, wParam, lParam );
}
