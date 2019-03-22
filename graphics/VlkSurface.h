#pragma once
#include "Core/Defines.h"
#include "Core/Types.h"
#include "GraphicsDecl.h"

#include <vector>
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

		void Init( VkSurfaceKHR pSurface, VlkPhysicalDevice* physicalDevice );

		bool CanPresent()const;

		uint32 GetPresentModeCount(VkPhysicalDevice pPhysicalDevice) const;
		const VkSurfaceCapabilitiesKHR& GetCapabilities() const;

		void GetPresentModes( VkPhysicalDevice pPhysicalDevice, uint32 modeCount, VkPresentModeKHR* presentModes ) const;

		uint32 GetFormatCount( ) const;
		VkSurfaceFormatKHR* GetSurfaceFormat();

	private:
		bool m_CanPresent = false;
		VkSurfaceKHR m_Surface = nullptr;

		VkSurfaceCapabilitiesKHR m_Capabilities;

		uint32 m_FormatCount = 0;
		VkSurfaceFormatKHR* m_Formats = nullptr;
		
		uint32 m_PresentCount = 0;
		VkPresentModeKHR* m_PresentModes = nullptr;

	};
}; // namespace Graphics