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

    void VlkSwapchain::Init( VlkDevice* device )
    {
    }

}; //namespace Graphics