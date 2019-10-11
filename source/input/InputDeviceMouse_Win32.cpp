#include "InputDeviceMouse_Win32.h"
#include <cassert>

namespace Input
{
	InputDeviceMouse_Win32::InputDeviceMouse_Win32( HWindow window_handle, HInstance instance_handle )
	{
		m_WindowHandle = window_handle;
		m_DeviceType = EDeviceType_Mouse;

		HRESULT hr =
			DirectInput8Create( instance_handle, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_Input, nullptr );
		assert( hr == S_OK && "Failed to create dinput8" );

		hr = m_Input->CreateDevice( GUID_SysMouse, &m_Device, nullptr );
		assert( hr == S_OK && "Failed to create input device!" );

		hr = m_Device->SetDataFormat( &c_dfDIMouse );
		assert( hr == S_OK && "Failed to set data format!" );

		hr = m_Device->SetCooperativeLevel( window_handle, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE );
		if( hr != S_OK )
		{
			assert( !"Failed to set cooperative level on device!" );
			Release();
			return;
		}

		hr = m_Device->Acquire();
		if( hr != S_OK )
		{
			assert( !"Failed to acquire input device!" );
			Release();
			return;
		}
	}

	InputDeviceMouse_Win32::~InputDeviceMouse_Win32() { Release(); }

	void InputDeviceMouse_Win32::Release()
	{
		m_Device->Unacquire();

		m_Device->Release();
		m_Device = nullptr;

		m_Input->Release();
		m_Input = nullptr;
	}

	bool InputDeviceMouse_Win32::OnDown() const { return false; }

	bool InputDeviceMouse_Win32::OnRelease() const { return false; }

	bool InputDeviceMouse_Win32::IsDown() const { return false; }

	bool InputDeviceMouse_Win32::OnDown( uint8 vkey ) const
	{
		return ( ( m_State.rgbButtons[vkey] & 0x80 ) != 0 ) && ( ( m_PrevState.rgbButtons[vkey] & 0x80 ) == 0 );
	}

	bool InputDeviceMouse_Win32::OnRelease( uint8 vkey ) const
	{
		return ( ( m_State.rgbButtons[vkey] & 0x80 ) == 0 ) && ( ( m_PrevState.rgbButtons[vkey] & 0x80 ) != 0 );
	}

	bool InputDeviceMouse_Win32::IsDown( uint8 vkey ) const { return ( ( m_State.rgbButtons[vkey] & 0x80 ) != 0 ); }

	void InputDeviceMouse_Win32::Update()
	{
		memcpy_s( &m_PrevState, sizeof( DIMOUSESTATE ), &m_State, sizeof( DIMOUSESTATE ) );
		if( m_Device->GetDeviceState( sizeof( DIMOUSESTATE ), (void**)&m_State ) != S_OK )
		{
			ZeroMemory( &m_State, sizeof( DIMOUSESTATE ) );
			if( m_Device->Acquire() != S_OK )
			{
				assert( !"Failed to acquire mouse device!" );
			}
		}

		POINT cursor_point;
		GetPhysicalCursorPos( &cursor_point );
		ScreenToClient( m_WindowHandle, &cursor_point );
		m_Cursor.x = (float)cursor_point.x;
		m_Cursor.y = (float)cursor_point.y;

		m_Cursor.dx = (float)m_PrevState.lX;
		m_Cursor.dy = (float)m_PrevState.lY;
	}

}; // namespace Input
