#include "InputDeviceKeyboard_Win32.h"
#include <cassert>
#include <dinput.h>

namespace Input
{
	InputDeviceKeyboard_Win32::InputDeviceKeyboard_Win32( HWindow window_handle, HInstance instance_handle )
	{
		m_DeviceType = EDeviceType_Keyboard;

		HRESULT hr =
			DirectInput8Create( instance_handle, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_Input, nullptr );
		assert( hr == S_OK && "Failed to create dinput8" );

		hr = m_Input->CreateDevice( GUID_SysKeyboard, &m_Device, nullptr );
		assert( hr == S_OK && "Failed to create input device!" );

		hr = m_Device->SetDataFormat( &c_dfDIKeyboard );
		assert( hr == S_OK && "Failed to set data format!" );

		hr = m_Device->SetCooperativeLevel( window_handle, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE );
		if( hr != S_OK )
		{
			assert( !"Failed to set cooperative level on device!" );
			Release();
			return;
		}

		if( m_Device->Acquire() != S_OK )
		{
			assert( !"Failed to acquire input device!" );
			Release();
			return;
		}
	}

	InputDeviceKeyboard_Win32::~InputDeviceKeyboard_Win32() { Release(); }

	void InputDeviceKeyboard_Win32::Release()
	{
		m_Device->Unacquire();

		m_Device->Release();
		m_Device = nullptr;

		m_Input->Release();
		m_Input = nullptr;
	}

	bool InputDeviceKeyboard_Win32::OnDown() const { return false; }

	bool InputDeviceKeyboard_Win32::OnRelease() const { return false; }

	bool InputDeviceKeyboard_Win32::IsDown() const { return false; }

	bool InputDeviceKeyboard_Win32::OnDown( uint8 vkey ) const
	{
		return ( ( m_State[vkey] & 0x80 ) != 0 ) && ( ( m_PrevState[vkey] & 0x80 ) == 0 );
	}

	bool InputDeviceKeyboard_Win32::OnRelease( uint8 vkey ) const
	{
		return ( ( m_State[vkey] & 0x80 ) == 0 ) && ( ( m_PrevState[vkey] & 0x80 ) != 0 );
	}

	bool InputDeviceKeyboard_Win32::IsDown( uint8 vkey ) const { return ( ( m_State[vkey] & 0x80 ) != 0 ); }

	void InputDeviceKeyboard_Win32::Update()
	{
		memcpy_s( &m_PrevState, sizeof( m_PrevState ), &m_State, sizeof( m_State ) );
		if( m_Device->GetDeviceState( sizeof( m_State ), (void**)&m_State ) != S_OK )
		{
			ZeroMemory( m_State, sizeof( m_State ) );
			if( m_Device->Acquire() != S_OK )
			{
			}
		}
	}

}; // namespace Input
