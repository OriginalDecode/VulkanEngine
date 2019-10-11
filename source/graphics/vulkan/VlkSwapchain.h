#pragma once

#include "Core/Defines.h"
#include <memory>
#include <vulkan/vulkan_core.h>
#include <vector>
LPE_DEFINE_HANDLE( VkSwapchainKHR );
LPE_DEFINE_HANDLE( VkSurfaceKHR );

//typedef void HImage;

struct VkSurfaceCapabilitiesKHR;
struct VkSurfaceFormatKHR;

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
		VlkSwapchain();
		~VlkSwapchain();
		void Release();

		VkSurfaceFormatKHR GetFormat( );
		void Init( VlkInstance* instance, VlkDevice* device, VlkPhysicalDevice* physicalDevice, const Window& window );
		size_t GetNofImages() const { return m_Images.size(); }

		std::vector<VkImage>& GetImageList() { return m_Images; }
		std::vector<VkImageView>& GetImageViewList() { return m_ImageViews; }
		VkSwapchainKHR GetSwapchain() { return m_Swapchain; }
		VkExtent2D GetExtent() const;

		VlkSurface* GetSurface();

	private:

		std::unique_ptr<VlkSurface> m_Surface;

		VkSwapchainKHR m_Swapchain;

		std::vector<VkImage> m_Images;

		std::vector<VkImageView> m_ImageViews;

	};

}; //namespace Graphics