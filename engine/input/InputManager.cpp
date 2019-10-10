#include "InputManager.h"

#include "InputDeviceKeyboard_Win32.h"
#include "InputDeviceMouse_Win32.h"

#include <cassert>

namespace Input
{
	InputManager* InputManager::m_Instance = nullptr;

	void InputManager::Initialize( HWindow window_handle, HInstance window_instance )
	{
		m_Devices.push_back( new HInputDeviceKeyboard( window_handle, window_instance ) );
		m_Devices.push_back( new HInputDeviceMouse( window_handle, window_instance ) );
	}

	void InputManager::Update()
	{
		for( const auto& device : m_Devices )
		{
			device->Update();
		}
	}

	void InputManager::AddDevice( IInputDevice* input_device ) { m_Devices.push_back( input_device ); }

	InputManager& InputManager::Get()
	{
		if( !m_Instance )
			m_Instance = new InputManager;

		return *m_Instance;
	}

	void InputManager::Destroy()
	{
		delete m_Instance;
		m_Instance = nullptr;
	}

}; // namespace Input
