#include "VlkInstance.h"
#include "VlkSurface.h"
#include "VlkPhysicalDevice.h"

#include <cassert>
#include <vulkan/vulkan.h>
#ifdef _WIN32
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif

#include <vector>

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
	VkDebugReportFlagsEXT /* flags */,
	VkDebugReportObjectTypeEXT /* objectType */,
	uint64_t /* object */,
	size_t /* location */,
	int32_t /* messageCode */,
	const char* /* pLayerPrefix */,
	const char* pMessage,
	void* /* pUserData */ )
{
	/*char temp[USHRT_MAX] = { 0 };
	sprintf_s( temp, "Warning : %s", pMessage );*/
	OutputDebugStringA(pMessage);
	OutputDebugStringA("\n");
	//assert( false && temp );
	return VK_FALSE;
}

namespace Graphics
{
	constexpr char* validationLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
	constexpr char* extentions[] = { "VK_KHR_surface", "VK_KHR_win32_surface", "VK_EXT_debug_report" };
	VkDebugReportCallbackEXT debugCallback;

	void SetupDebugCallback( VkInstance instance )
	{
		auto FCreateCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr( instance, "vkCreateDebugReportCallbackEXT" );
		assert( FCreateCallback && "Failed to setup callback!" );

		if( !FCreateCallback )
			return;

		VkDebugReportFlagsEXT flags =
			VK_DEBUG_REPORT_ERROR_BIT_EXT |
			VK_DEBUG_REPORT_WARNING_BIT_EXT |
			VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;

		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.pfnCallback = &DebugReportCallback;
		createInfo.flags = flags;

		VkResult result = FCreateCallback( instance, &createInfo, nullptr, &debugCallback );
		assert( result == VK_SUCCESS );
	}

	VlkInstance::~VlkInstance()
	{
		Release();
	}

	void VlkInstance::Init()
	{
		VkApplicationInfo appInfo = {};

		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Skold Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
		appInfo.pEngineName = "Skold Engine";
		appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
		appInfo.apiVersion = VK_API_VERSION_1_1;

		VkInstanceCreateInfo instanceCreateInfo = {};

		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = 0;

		instanceCreateInfo.enabledLayerCount = ARRSIZE( validationLayers );
		instanceCreateInfo.ppEnabledLayerNames = validationLayers;

		instanceCreateInfo.enabledExtensionCount = ARRSIZE( extentions );
		instanceCreateInfo.ppEnabledExtensionNames = extentions;

		VkResult result = vkCreateInstance( &instanceCreateInfo, nullptr /*allocator*/, &m_Instance );
		assert( result == VK_SUCCESS && "Failed to create Vulkan instance!" );

		SetupDebugCallback( m_Instance );
	}

	void VlkInstance::Release()
	{
		vkDestroyInstance( m_Instance, nullptr );
	}

	VkSurfaceKHR VlkInstance::CreateSurface( const VkWin32SurfaceCreateInfoKHR& createInfo ) const
	{
		VkSurfaceKHR surface = nullptr;
		VkResult result = vkCreateWin32SurfaceKHR( m_Instance, &createInfo, nullptr, &surface );
		assert( result == VK_SUCCESS );

		return surface;
	}

	upVlkSurface VlkInstance::CreateSurface( const VkWin32SurfaceCreateInfoKHR& createInfo, VlkPhysicalDevice* physicalDevice ) const
	{
		upVlkSurface surface = std::make_unique<VlkSurface>();
		surface->Init( CreateSurface( createInfo ), physicalDevice );
		return surface;
	}

	void VlkInstance::DestroySurface( VkSurfaceKHR pSurface )
	{
		vkDestroySurfaceKHR( m_Instance, pSurface, nullptr );
	}

	void VlkInstance::GetPhysicalDevices(std::vector<VkPhysicalDevice>& deviceList)
	{
		uint32 device_count = 0;
		VkResult result = vkEnumeratePhysicalDevices(m_Instance, &device_count, nullptr);
		assert(result == VK_SUCCESS && "Failed to enumerate device!");

		deviceList.resize(device_count);
		result = vkEnumeratePhysicalDevices(m_Instance, &device_count, deviceList.data());
		assert(result == VK_SUCCESS && "Failed to enumerate device!");

	}

}; // namespace Graphics