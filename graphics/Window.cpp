#include "Window.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <vector>


BOOL CALLBACK MonitorFound(HMONITOR /*hMonitor*/,
				  HDC /*hdcMonitor*/,
				  LPRECT lprcMonitor,
				  LPARAM dwData)
{

	std::vector<RECT>* arr = reinterpret_cast<std::vector<RECT>*>(dwData); 
	arr->push_back(*lprcMonitor);

	return TRUE;
}



Window::Window(const CreateInfo& info)
{
	m_WindowSize = info.m_Size;
#ifdef _WIN32
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)info.m_WindowProcess;
	wc.hInstance = info.m_InstanceHandle;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "WindowsClass";
	RegisterClassEx(&wc);

	const int count_monitors = GetSystemMetrics(SM_CMONITORS);

	std::vector<RECT> monitors;
	EnumDisplayMonitors(nullptr, nullptr, &MonitorFound, (LPARAM)&monitors);

	const int monitor_index = 0; //current window index. Should be read from a setting, preferably changeable during runtime. Connect graphics device to the window somehow?

	const float width = (float)monitors[monitor_index].right - monitors[monitor_index].left; // width of the monitor
	const float height = (float)monitors[monitor_index].bottom - monitors[monitor_index].top; // height of the monitor
	
	// calculate the center position of the window on the monitor
	const int32 window_x = int32(float(monitors[monitor_index].left + (width / 2.f)) - m_WindowSize.m_Width / 2.f);
	const int32 window_y = int32(float(monitors[monitor_index].top + (height / 2.f)) - m_WindowSize.m_Height / 2.f);


	m_WindowHandle = CreateWindow(
		"WindowsClass"
		, nullptr
		, WS_OVERLAPPEDWINDOW   //WS_POPUP, Borderless windowed
		, window_x 
		, window_y
		, int32(m_WindowSize.m_Width)
		, int32(m_WindowSize.m_Height)
		, nullptr
		, nullptr
		, GetModuleHandle(nullptr), nullptr);

	RECT inner_size;
	GetClientRect(m_WindowHandle, &inner_size);
	m_InnerSize = { (float)inner_size.bottom, (float)inner_size.right };
#else

#endif

	ShowWindow();
}

Window::~Window()
{
}

void Window::ShowWindow()
{
#ifdef _WIN32
	::ShowWindow(m_WindowHandle, true);
#endif

}

void Window::SetText(const char* window_text)
{
	::SetWindowTextA(m_WindowHandle, window_text);
}
