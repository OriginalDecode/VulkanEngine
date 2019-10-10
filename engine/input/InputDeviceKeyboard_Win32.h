#pragma once

#include "InputDevice.h"

namespace Input
{
	class InputDeviceKeyboard_Win32 final : public IInputDevice
	{
	public:
		InputDeviceKeyboard_Win32( HWindow window_handle, HInstance instance_handle );
		~InputDeviceKeyboard_Win32() override;

		bool OnDown() const override;
		bool OnRelease() const override;
		bool IsDown() const override;

		bool OnDown( uint8 vkey ) const override;
		bool OnRelease( uint8 vkey ) const override;
		bool IsDown( uint8 vkey ) const override;

		void Update() override;

	private:
		void Release() override;
		uint8 m_State[256]{ 0 };
		uint8 m_PrevState[256]{ 0 };
		IDirectInput8* m_Input{ nullptr };
		IDirectInputDevice8* m_Device{ nullptr };
	};

	using HInputDeviceKeyboard = InputDeviceKeyboard_Win32;
}; // namespace Input