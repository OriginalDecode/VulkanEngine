#include "VlkSwapchain.h"

#include "VlkDevice.h"
#include "VlkInstance.h"
#include "VlkSurface.h"
#include "VlkPhysicalDevice.h"
#include "Window.h"

#include "GraphicsEngine.h"
#include "vkGraphicsDevice.h"

#include <vulkan/vulkan_core.h>
#include <windows.h>
#include <vulkan/vulkan_win32.h>

#include <cassert>

namespace Graphics
{

	VlkSwapchain::VlkSwapchain() = default;
	VlkSwapchain::~VlkSwapchain() 
	{
		Release();
	}

	void VlkSwapchain::Release()
	{
		VlkDevice& device = GraphicsEngine::GetDevice().GetVlkDevice();
		device.DestroySwapchain( m_Swapchain );
	}


	void VlkSwapchain::Init( VlkInstance* instance, VlkDevice* device, VlkPhysicalDevice* physicalDevice, const Window& window )
	{
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = window.GetHandle();
		createInfo.hinstance = GetModuleHandle( nullptr );

		m_Surface = instance->CreateSurface( createInfo, physicalDevice );
		QueueProperties queueProp = physicalDevice->FindFamilyIndices( m_Surface.get() );

		if( !m_Surface->CanPresent() )
		{
			assert( !"Surface cannot present!" );
			return;
		}


		const VkSurfaceCapabilitiesKHR& capabilities = m_Surface->GetCapabilities();

		uint32 swapchain_image_count = 2;
		assert( swapchain_image_count < capabilities.maxImageCount );

		VkExtent2D extent = capabilities.currentExtent;

		
		const std::vector<VkSurfaceFormatKHR>& formats = m_Surface->GetSurfaceFormats();
		VkSurfaceFormatKHR format = formats[0];
		if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
		{
			format = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}
		else
		{
			for (const auto& availableFormat : formats)
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					format = availableFormat;
				}
			}
		}





		VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.surface = m_Surface->m_Surface;
		swapchainCreateInfo.minImageCount = capabilities.minImageCount;
		swapchainCreateInfo.imageFormat = format.format;
		swapchainCreateInfo.imageColorSpace = format.colorSpace;
		swapchainCreateInfo.imageExtent = extent;
		swapchainCreateInfo.imageArrayLayers = 1;

		if( queueProp.queueIndex != queueProp.familyIndex )
		{
			uint32_t queueIndices[] = { (uint32_t)queueProp.queueIndex, (uint32_t)queueProp.familyIndex };
			swapchainCreateInfo.queueFamilyIndexCount = ARRSIZE(queueIndices);
			swapchainCreateInfo.pQueueFamilyIndices = queueIndices;
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		}
		else
		{
			swapchainCreateInfo.queueFamilyIndexCount = 0;
			swapchainCreateInfo.pQueueFamilyIndices = nullptr;
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		VkSurfaceTransformFlagBitsKHR transformFlags = capabilities.currentTransform;
		if( capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR )
			transformFlags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

		swapchainCreateInfo.preTransform = transformFlags;

		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		swapchainCreateInfo.clipped = VK_TRUE;


		m_Swapchain = device->CreateSwapchain( swapchainCreateInfo );
	}

	void VlkSwapchain::GetSurfaceCapabilities( VkSurfaceKHR /*surface*/, const VlkPhysicalDevice& /*physicalDevice*/, VkSurfaceCapabilitiesKHR* /*pCapabilities */)
	{

		//VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR( surface, 0, pCapabilities );
	}

}; //namespace Graphics