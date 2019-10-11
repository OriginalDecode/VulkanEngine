#pragma once
#include "core/Defines.h"
#include "core/Types.h"
#include "graphics/IGfxDevice.h"

#include <vulkan/vulkan_core.h>

LPE_DEFINE_HANDLE( VkDevice );
LPE_DEFINE_HANDLE( VkQueue );
LPE_DEFINE_HANDLE( VkSwapchainKHR );
LPE_DEFINE_HANDLE( VkSurfaceKHR );

struct VkSwapchainCreateInfoKHR;
struct VkWin32SurfaceCreateInfoKHR;

namespace Graphics
{
	class VlkPhysicalDevice;
	class VulkanDevice
	{
	public:
		VulkanDevice() = default;
		~VulkanDevice();

		void Init();

		VkDevice GetDevice() const { return m_Device; }
		VkQueue GetQueue() const { return m_Queue; }

		VkSwapchainKHR CreateSwapchain( const VkSwapchainCreateInfoKHR& createInfo ) const;
		void DestroySwapchain( VkSwapchainKHR pSwapchain );

		//void GetSwapchainImages( VkSwapchainKHR* pSwapchain, std::vector<VkImage>* scImages );

	private:
		void CreateInstance();
		void CreatePhysicalDevice();
		void CreateDevice();


		VkInstance m_Instance{ nullptr };
		VkPhysicalDevice m_PhysicalDevice{ nullptr };
		VkDevice m_Device{ nullptr };
		VkQueue m_Queue{ nullptr };

		uint32 m_QueueFamily{ 0 };
		uint32 m_PresentFamily{ 0 };

		//std::vector<VkQueueFamilyProperties> m_QueueProperties;
	};

}; //namespace Graphics