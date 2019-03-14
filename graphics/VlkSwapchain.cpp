#include "VlkSwapchain.h"
#include <vulkan/vulkan_core.h>
#include "VlkDevice.h"
namespace Graphics
{
 void VlkSwapchain::Release( IGfxDevice* device )
	{
		vkDestroySwapchainKHR( static_cast<VlkDevice*>(device)->GetDevice(), m_Swapchain, nullptr );
	}

	VlkSwapchain::~VlkSwapchain()
	{
	
	}

	void VlkSwapchain::Init(const VlkDevice& device, const VlkPhysicalDevice& physicalDevice, const Window& window)
	{

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = REPLACE_ME; // replace with a real surface
		createInfo.minImageCount = REPLACE_ME; //replace with the correct one
		createInfo.imageFormat = VK_FORMAT_R8G8B8_UNORM;
		createInfo.imageColorSpace = REPLACE_ME;
		createInfo.imageExtent = REPLACE_ME;
		createInfo.imageArrayLayers = 1;

		createInfo.queueFamilyIndexCount = REPLACE_ME;
		createInfo.pQueueFamilyIndices = REPLACE_ME;

		createInfo.preTransform = REPLACE_ME;

		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		createInfo.clipped = VK_TRUE;


		m_Swapchain = device.CreateSwapchain( /*create info*/ );
	}

}; //namespace Graphics