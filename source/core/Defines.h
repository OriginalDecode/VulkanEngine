#pragma once

#define SAFE_DELETE(ptr) \
	delete ptr;          \
	ptr = nullptr;
#define SAFE_DELETEA(array) \
	delete[] array;         \
	array = nullptr;

#define BIT(x) (1 << x)

#define DEFINE_HANDLE(object) typedef struct object##_T* object;
#define ARRSIZE(x) sizeof(x) / sizeof(x[0])

#define MAKE_VERSION(major, minor, patch) (((major) << 22) | ((minor) << 12) | (patch))

#ifdef _WIN32
#ifndef _WINDEF_
struct HINSTANCE__;
typedef HINSTANCE__* HINSTANCE;
struct HWND__;
typedef HWND__* HWND;
#endif
typedef HWND HWindow;
typedef HINSTANCE HInstance;
#else
typedef void* HWindow; // X11 Window
typedef void* HInstance;
#endif
