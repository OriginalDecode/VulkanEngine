#include "Game.h"

#include "Input/InputManager.h"
#include "input/InputDeviceMouse_Win32.h"
#include "input/InputDeviceKeyboard_Win32.h"

#include "game/camera/Camera.h"

void Game::InitState(StateStack* state_stack)
{
	m_StateStack = state_stack;

	m_Camera = new Camera;
}

void Game::EndState()
{
	delete m_Camera;
	m_Camera = nullptr;
}

void Game::Update(float /*dt*/)
{
	// 	Input::InputManager& input = Input::InputManager::Get();
	//
	// 	Input::HInputDeviceMouse* mouse = nullptr;
	// 	input.GetDevice( Input::EDeviceType_Mouse, &mouse );
	//
	// 	const Input::Cursor& cursor = mouse->GetCursor();
	//
	// 	Input::HInputDeviceKeyboard* keyboard = nullptr;
	// 	input.GetDevice( Input::EDeviceType_Keyboard, &keyboard );
}

void Game::Render(bool /*render_through*/) {}
