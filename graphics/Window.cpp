#include "Window.h"

#include "Core/utilities/utilities.h"

#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

struct MonitorCallbackData
{
    RECT m_Rect;
    bool m_IsPrimary = false;
};

BOOL CALLBACK MonitorFound( HMONITOR hMonitor,
                            HDC /*hdcMonitor*/,
                            LPRECT lprcMonitor,
                            LPARAM dwData )
{
    MonitorCallbackData data;
    MONITORINFO info;
    info.cbSize = sizeof( MONITORINFO );

    if( GetMonitorInfo( hMonitor, &info ) != 0 )
    {
        if( info.dwFlags & MONITORINFOF_PRIMARY )
        {
            data.m_IsPrimary = true;
        }
    }
    else
    {
        Core::DebugPrintLastError();
    }

    std::vector<MonitorCallbackData>* arr = reinterpret_cast<std::vector<MonitorCallbackData>*>( dwData );

    data.m_Rect = *lprcMonitor;

    arr->push_back( data );

    return TRUE;
}

Window::Window( CreateInfo info )
{
    m_WindowSize = info.m_Size;
#ifdef _WIN32
    WNDCLASSEX wc;
    ZeroMemory( &wc, sizeof( WNDCLASSEX ) );
    wc.cbSize = sizeof( WNDCLASSEX );
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)info.m_WindowProcess;
    wc.hInstance = info.m_InstanceHandle;
    wc.hCursor = LoadCursor( nullptr, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = L"WindowsClass";
    RegisterClassEx( &wc );

    const int count_monitors = GetSystemMetrics( SM_CMONITORS );

    std::vector<MonitorCallbackData> monitors;
    EnumDisplayMonitors( nullptr, nullptr, &MonitorFound, (LPARAM)&monitors );

    int monitor_index = 0; //current window index. Should be read from a setting, preferably changeable during runtime. Connect graphics device to the window somehow?
    for( const auto& data : monitors )
    {
        if( data.m_IsPrimary )
            break;

        monitor_index++;
    }

    const float width = (float)monitors[monitor_index].m_Rect.right - monitors[monitor_index].m_Rect.left;  // width of the monitor
    const float height = (float)monitors[monitor_index].m_Rect.bottom - monitors[monitor_index].m_Rect.top; // height of the monitor

    // calculate the center position of the window on the monitor
    const int32 window_x = int32( float( monitors[monitor_index].m_Rect.left + ( width / 2.f ) ) - m_WindowSize.m_Width / 2.f );
    const int32 window_y = int32( float( monitors[monitor_index].m_Rect.top + ( height / 2.f ) ) - m_WindowSize.m_Height / 2.f );

    m_WindowHandle = CreateWindow(
        L"WindowsClass", nullptr, WS_OVERLAPPEDWINDOW //WS_POPUP, Borderless windowed
        ,
        window_x, window_y, int32( m_WindowSize.m_Width ), int32( m_WindowSize.m_Height ), nullptr, nullptr, GetModuleHandle( nullptr ), nullptr );

    RECT inner_size;
    GetClientRect( m_WindowHandle, &inner_size );
    m_InnerSize = { (float)inner_size.right, (float)inner_size.bottom };
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
    ::ShowWindow( m_WindowHandle, true );
#endif
}

void Window::SetText( const char* window_text )
{
    ::SetWindowTextA( m_WindowHandle, window_text );
}
