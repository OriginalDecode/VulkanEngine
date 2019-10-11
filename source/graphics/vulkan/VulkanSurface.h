#pragma once

#include "Core/Defines.h"
#include "Core/Types.h"
#include "graphics/GraphicsDecl.h"

#include <vulkan/vulkan_core.h>

// This probably doesn't

LPE_DEFINE_HANDLE( VkSurfaceKHR );
LPE_DEFINE_HANDLE( VkPhysicalDevice );
struct VkWin32SurfaceCreateInfoKHR;
/* put into swapchain??? */
namespace Graphics
{
	class VulkanSurface
	{
	public:
		VulkanSurface();
		~VulkanSurface();
		
		void Init( VkSurfaceKHR pSurface, VlkPhysicalDevice* physicalDevice );

		bool CanPresent() const;

		const VkSurfaceCapabilitiesKHR& GetCapabilities() const;

		const VkSurfaceFormatKHR* GetSurfaceFormats() const;
		uint32 GetFormatCount() const { return m_NofFormats; }

		VkSurfaceKHR GetSurface() { return m_Surface; }

	private:
		bool m_CanPresent{ false };
		VkSurfaceKHR m_Surface{ nullptr };

		VkSurfaceCapabilitiesKHR m_Capabilities;

		uint32 m_NofFormats{ 0 };
		VkSurfaceFormatKHR* m_Formats{ nullptr };

		uint32 m_NofModes{ 0 };
		VkPresentModeKHR* m_Modes{ nullptr };
	};
}; // namespace Graphics