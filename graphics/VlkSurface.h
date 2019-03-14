#pragma once
#include "Core/Defines.h"
#include "Core/Types.h"


#include <vulkan/vulkan_core.h>

LPE_DEFINE_HANDLE( VkSurfaceKHR );
LPE_DEFINE_HANDLE( VkPhysicalDevice );

struct VkWin32SurfaceCreateInfoKHR;

namespace Graphics
{
	class VlkInstance;
	class VlkPhysicalDevice;
	class VlkSurface
	{
		friend class VlkSwapchain;
	public:
		VlkSurface() = default;
		~VlkSurface();
		void Release();

		void Init( VkSurfaceKHR pSurface );

		bool CanPresent( const VlkPhysicalDevice& physicalDevice )const;

		uint32 GetPresentModeCount(VkPhysicalDevice pPhysicalDevice) const;
		void GetPresentModes( VkPhysicalDevice pPhysicalDevice, uint32 modeCount, VkPresentModeKHR* presentModes ) const;
		VkSurfaceCapabilitiesKHR GetCapabilities( VkPhysicalDevice pPhysicalDevice ) const;

	private:
		VkSurfaceKHR m_Surface = nullptr;
	};
}; // namespace Graphics