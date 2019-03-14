#pragma once

#include "Core/Defines.h"
#include <memory>
LPE_DEFINE_HANDLE( VkSwapchainKHR );
LPE_DEFINE_HANDLE( VkSurfaceKHR );
struct VkSurfaceCapabilitiesKHR;

class Window;
namespace Graphics
{
	class VlkDevice;
	class VlkPhysicalDevice;
	class VlkInstance;
	class VlkSurface;

	class VlkSwapchain 
	{
	public:
		VlkSwapchain() = default;
		~VlkSwapchain();
		void Release();

		void Init( const VlkInstance& instance, const VlkDevice& device, const VlkPhysicalDevice& physicalDevice, const Window& window );

	private:
		void GetSurfaceCapabilities( VkSurfaceKHR surface, const VlkPhysicalDevice& physicalDevice, VkSurfaceCapabilitiesKHR* pCapabilities );

		std::unique_ptr<VlkSurface> m_Surface;
		VkSwapchainKHR m_Swapchain;
	};

}; //namespace Graphics