#pragma once
#include "InputDevice.h"
#include <Core/Defines.h>
#include <vector>

namespace Input
{
	class InputManager
	{
	public:
		InputManager() = default;
		~InputManager() = default;

		void Initialize( HWindow windowHandle, HInstance windowInstance );

		/* update the inputs */
		void Update();

		/* add a device to the device list */
		void AddDevice( IInputDevice* inputDevice );

		/* get a specific device */
		template <typename T>
		void GetDevice( EDeviceType deviceType, T** deviceOut );

		static InputManager& Get();
		static void Destroy();

	private:
		static InputManager* m_Instance;
		std::vector<IInputDevice*> m_Devices{};
	};

	template <typename T>
	void Input::InputManager::GetDevice( EDeviceType deviceType, T** deviceOut )
	{
		assert( deviceType != EDeviceType_Gamepad && "No implementation for getting a gamepad yet. Need to retrieve a "
													 "list of multiple devices to be able to retrieve this" );

		if( deviceType != EDeviceType_Gamepad )
		{
			for( size_t i = 0; i < m_Devices.size(); ++i )
			{
				Input::IInputDevice* device = m_Devices[i];
				if( device->GetType() == deviceType )
				{
					*deviceOut = static_cast<T*>( device );
				}
			}
		}
	}

}; // namespace Input
