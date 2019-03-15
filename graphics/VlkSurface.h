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

		void Init( VkSurfaceKHR pSurface, const VlkPhysicalDevice& physicalDevice );

		bool CanPresent()const;

		uint32 GetPresentModeCount(VkPhysicalDevice pPhysicalDevice) const;
		VkSurfaceCapabilitiesKHR GetCapabilities( const VlkPhysicalDevice& physicalDevice ) const;

		void GetPresentModes( VkPhysicalDevice pPhysicalDevice, uint32 modeCount, VkPresentModeKHR* presentModes ) const;

		uint32 GetFormatCount( VkPhysicalDevice pPhysicalDevice ) const;
		void GetSurfaceFormat( VkPhysicalDevice pPhysicalDevice, VkSurfaceFormatKHR* formats );

	private:
		bool m_CanPresent = false;
		VkSurfaceKHR m_Surface = nullptr;
		


	};
}; // namespace Graphics