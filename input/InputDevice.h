#pragma once
#include <Core/Types.h>
#ifdef _WIN32
#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )
struct IDirectInputDevice8A;
typedef IDirectInputDevice8A IDirectInputDevice8;

struct IDirectInput8A;
typedef IDirectInput8A IDirectInput8;

#ifndef _WINDEF_
struct HINSTANCE__;
typedef HINSTANCE__* HINSTANCE;
struct HWND__;
typedef HWND__* HWND;
#endif

#else

#endif

namespace Input
{

	enum EDeviceType
	{
		EDeviceType_Unknown,
		EDeviceType_Keyboard,
		EDeviceType_Mouse,
		EDeviceType_Gamepad
	};

	class IInputDevice
	{
	public:
		virtual ~IInputDevice() {}

		virtual bool OnDown() const = 0;
		virtual bool OnRelease() const = 0;
		virtual bool IsDown() const = 0;

		virtual void Update() = 0;

		virtual bool OnDown(uint8 vkey) const = 0;
		virtual bool OnRelease(uint8 vkey) const = 0;
		virtual bool IsDown(uint8 vkey) const = 0;




		const EDeviceType GetType() const { return m_DeviceType; }

	protected:
		virtual void Release() = 0;
		EDeviceType m_DeviceType{ EDeviceType_Unknown };
	};

}; // namespace Input