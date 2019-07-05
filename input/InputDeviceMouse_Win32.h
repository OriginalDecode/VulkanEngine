#pragma once

#include "InputDevice.h"

#include <dinput.h>

namespace Input
{
	class InputDeviceMouse_Win32 final : public IInputDevice
	{
	public:
		InputDeviceMouse_Win32( HWindow window_handle, HInstance instance_handle );
		~InputDeviceMouse_Win32() override;

		bool OnDown() const override;
		bool OnRelease() const override;
		bool IsDown() const override;

		bool OnDown( uint8 vkey ) const override;
		bool OnRelease( uint8 vkey ) const override;
		bool IsDown( uint8 vkey ) const override;

		void Update() override;

	private:
		void Release() override;
		Cursor m_Cursor;
		DIMOUSESTATE2 m_State;
		DIMOUSESTATE2 m_PrevState;
		IDirectInput8* m_Input = nullptr;
		IDirectInputDevice8* m_Device = nullptr;
		HWindow m_WindowHandle = nullptr;
	};
	using HInputDeviceMouse = InputDeviceMouse_Win32;
}; // namespace Input
