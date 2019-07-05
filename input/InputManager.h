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
		IInputDevice* GetDevice( EDeviceType deviceType );

		static InputManager& Get();
		static void Destroy();

	private:
		static InputManager* m_Instance;
		std::vector<IInputDevice*> m_Devices{};
	};
}; // namespace Input
