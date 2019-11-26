#include "GraphicsEngineVulkanImpl.h"
#include "RenderContextVulkan.h"
#include "graphics/Window.h"
#include "graphics/vulkan/VulkanUtils.h"

#include "logger/Debug.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

namespace Graphics
{
	void GraphicsEngineVulkanImpl::Init(const Window& window)
	{
		m_Context = new RenderContextVulkan;
		m_Context->Window = &window;
		vlk::CreateInstance("wce", VK_MAKE_VERSION(1, 0, 0), "wce", VK_MAKE_VERSION(1, 0, 0),
							VK_VERSION_1_1, m_Context);

		CreatePhysicalDevice();
		CreateLogicalDevice();
		CreateSwapchain();
		// Create Depth
	}

	void GraphicsEngineVulkanImpl::CreatePhysicalDevice()
	{
		auto [device_list, device_count] = vlk::AllocPhysicalDeviceList(m_Context->Instance);

		int32 queue_family = 0;
		bool family_found = false;
		for(int32 i = 0; i < device_count; ++i)
		{
			VkPhysicalDevice device = device_list[i];

			VkPhysicalDeviceProperties device_properties = {};
			vkGetPhysicalDeviceProperties(device, &device_properties);

			VkPhysicalDeviceFeatures device_features = {};
			vkGetPhysicalDeviceFeatures(device, &device_features);

			if(device_properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
			   !device_features.geometryShader)
				continue;

			auto [properties, property_count] = vlk::AllocPhysicalDevicePropertiesList(device);
			for(int32 j = 0; j < property_count; ++j)
			{
				VkQueueFamilyProperties property = properties[j];
				if(property.queueCount > 0 && property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					queue_family = j;
					family_found = true;
					break;
				}
			}

			if(family_found)
			{
				m_Context->PhysicalDevice = device;
				m_Context->QueueFamily = (uint32)queue_family;
				vlk::FreePhysicalDevicePropertiesList(properties);
				break;
			}
		}

		vlk::FreePhysicalDeviceList(device_list);
	}

	void GraphicsEngineVulkanImpl::CreateLogicalDevice()
	{
		// Create logical device
		const float queue_priorities[] = { 1.f };
		VkDeviceQueueCreateInfo queue_create_info = {};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = m_Context->QueueFamily;
		queue_create_info.queueCount = 1;
		queue_create_info.pQueuePriorities = queue_priorities;

		// Get the enabled features from the physical device
		VkPhysicalDeviceFeatures enabled_features = {};
		vkGetPhysicalDeviceFeatures(m_Context->PhysicalDevice, &enabled_features);

		// Device create info
		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = 1;
		create_info.pQueueCreateInfos = &queue_create_info;

#ifdef _DEBUG
		const char* debugLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
		create_info.enabledLayerCount = ARRSIZE(debugLayers);
		create_info.ppEnabledLayerNames = debugLayers;
#endif

		const char* device_ext[] = { "VK_KHR_swapchain" };
		create_info.enabledExtensionCount = ARRSIZE(device_ext);
		create_info.ppEnabledExtensionNames = device_ext;
		create_info.pEnabledFeatures = &enabled_features;

		if(vkCreateDevice(m_Context->PhysicalDevice, &create_info, nullptr, &m_Context->Device) !=
		   VK_SUCCESS)
			ASSERT(false, "Failed to create device!");
	}

	void GraphicsEngineVulkanImpl::CreateSwapchain()
	{

		VkWin32SurfaceCreateInfoKHR surface_info = {};
		surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surface_info.hwnd = (HWND)m_Context->Window->GetHandle();
		surface_info.hinstance = ::GetModuleHandle(nullptr);

		if(vkCreateWin32SurfaceKHR(m_Context->Instance, &surface_info, nullptr,
								   &m_Context->Surface) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to create surface!");
		}

		int32 queue_family = -1;
		int32 queue_index = -1;
		VkQueueFamilyProperties* queue_family_props = {};

		auto [properties, property_count] =
			vlk::AllocPhysicalDevicePropertiesList(m_Context->PhysicalDevice);

		for(int32 i = 0; i < property_count; ++i)
		{
			VkQueueFamilyProperties property = properties[i];

			if(property.queueCount > 0)
			{
				const bool can_present = vlk::CanPresent(m_Context->Surface);
				ASSERT(can_present, "No surface to present on provided when creating swapchain!");
				if(can_present)
				{
					queue_family = i;
				}

				if((property.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
				{
					queue_index = i;
				}

				if(queue_index >= 0 && queue_family >= 0)
					break;
			}
		}

		vlk::FreePhysicalDevicePropertiesList(properties);

		VkSurfaceCapabilitiesKHR surface_capabilities =
			vlk::GetSurfaceCapabilities(m_Context->Surface);
		const uint32 image_count = 2;
		ASSERT(image_count <= surface_capabilities.minImageCount, ""); // this assert is a bit
																	   // strange?
		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = m_Context->Surface;
		create_info.minImageCount = surface_capabilities.minImageCount;

		// Get the physical device surface format
		auto [format_list, nof_formats] =
			vlk::AllocPhysicalDeviceSurfaceFormatsList(m_Context->Surface);

		VkSurfaceFormatKHR format = vlk::GetSurfaceFormat(
			format_list, nof_formats, VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

		vlk::FreePhysicalDeviceSurfaceFormatsList(format_list);

		create_info.imageFormat = format.format;
		create_info.imageColorSpace = format.colorSpace;

		create_info.imageExtent = surface_capabilities.currentExtent;
		create_info.imageArrayLayers = 1;

		if(queue_index != queue_family)
		{
			const uint32_t queue_indices[] = { (uint32_t)queue_index, (uint32_t)queue_family };
			create_info.queueFamilyIndexCount = ARRSIZE(queue_indices);
			create_info.pQueueFamilyIndices = queue_indices;
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		}
		else
		{
			create_info.queueFamilyIndexCount = 0;
			create_info.pQueueFamilyIndices = nullptr;
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		VkSurfaceTransformFlagBitsKHR transform_flags = surface_capabilities.currentTransform;
		if(surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
			transform_flags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

		create_info.preTransform = transform_flags;

		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; // this field
																 // defines
																 // blocking or
																 // nonblocking -
																 // (VK_PRESENT_MODE_FIFO_KHR)
																 // blocks (vsync
																 // on or off)
		create_info.clipped = VK_TRUE;

		if(vkCreateSwapchainKHR(m_Context->Device, &create_info, nullptr,
								&m_Context->SwapchainCtx.Swapchain) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to create swapchain");
		}

		uint32 nof_images;
		if(vkGetSwapchainImagesKHR(m_Context->Device, m_Context->SwapchainCtx.Swapchain,
								   &nof_images, nullptr) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to get nof images from swapchain");
		}

		m_Context->SwapchainCtx.Images = new VkImage[nof_images];
		if(vkGetSwapchainImagesKHR(m_Context->Device, m_Context->SwapchainCtx.Swapchain,
								   &nof_images, m_Context->SwapchainCtx.Images) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to get images from swapchain");
		}

		m_Context->SwapchainCtx.Views = new VkImageView[nof_images];
		m_Context->SwapchainCtx.ImageCount = nof_images;
	}

}; // namespace Graphics
