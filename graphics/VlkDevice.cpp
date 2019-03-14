#include "VlkDevice.h"
#include "VlkPhysicalDevice.h"

#include <vulkan/vulkan_core.h>
#include <cassert>
namespace Graphics
{
	const char* debugLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
	const char* deviceExt[] = { "VK_KHR_swapchain" };

	VlkDevice::~VlkDevice()
	{
		Release(nullptr);
	}

	void VlkDevice::Release(IGfxDevice*)
	{
		vkDestroyDevice(m_Device, nullptr);
	}

	VkSwapchainKHR VlkDevice::CreateSwapchain(const VkSwapchainCreateInfoKHR& createInfo) const
	{
		VkSwapchainKHR swapchain = nullptr;
		VkResult result = vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &swapchain);
		assert(result == VK_SUCCESS && "Failed to create swapchain");
		return swapchain;
	}

	void VlkDevice::Init(const VlkPhysicalDevice& physicalDevice)
	{
		//queue create info
		const float queue_priorities[] = { 1.f };
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = physicalDevice.GetQueueFamilyIndex();
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = queue_priorities;

		//Physical device features
		VkPhysicalDeviceFeatures enabled_features = {};
		enabled_features.shaderClipDistance = true;

		// device create info
		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
#ifdef _DEBUG
		createInfo.enabledLayerCount = ARRSIZE(debugLayers);
		createInfo.ppEnabledLayerNames = debugLayers;
#endif
		createInfo.enabledExtensionCount = ARRSIZE(deviceExt);
		createInfo.ppEnabledExtensionNames = deviceExt;
		createInfo.pEnabledFeatures = &enabled_features;

		m_Device = physicalDevice.CreateDevice(createInfo);

		vkGetDeviceQueue(m_Device, physicalDevice.GetQueueFamilyIndex(), 0, &m_Queue);
	}
}; //namespace Graphics