#include "vkInstance.h"
#include <vulkan/vulkan.h>
#include <cassert>
#include <vector>

namespace Graphics
{
	constexpr char* validationLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
	constexpr char* extentions[] = { "VK_KHR_surface", "VK_KHR_win32_surface", "VK_EXT_debug_report" };
	
	vkInstance::~vkInstance()
	{
		Release();
	}

	void vkInstance::Init()
	{
		VkApplicationInfo appInfo = {};

		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Skold Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Skold Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_1;



		VkInstanceCreateInfo instanceCreateInfo = {};

		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = 0;

		instanceCreateInfo.enabledLayerCount = ARRSIZE(validationLayers);
		instanceCreateInfo.ppEnabledLayerNames = validationLayers;

		instanceCreateInfo.enabledExtensionCount = ARRSIZE(extentions);
		instanceCreateInfo.ppEnabledExtensionNames = extentions;



		VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr /*allocator*/, &m_Instance);
		assert(result == VK_SUCCESS && "Failed to create vulkan instance!");
	}

	void vkInstance::Release()
	{
		vkDestroyInstance(m_Instance, nullptr);
	}

};