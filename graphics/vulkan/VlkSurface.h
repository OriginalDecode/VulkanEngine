#pragma once
#include "Core/Defines.h"
#include "Core/Types.h"
#include "graphics/GraphicsDecl.h"

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
		VlkSurface();
		~VlkSurface();
		void Release();

		void Init( VkSurfaceKHR pSurface, VlkPhysicalDevice* physicalDevice );

		bool CanPresent()const;

		uint32 GetPresentModeCount(VkPhysicalDevice pPhysicalDevice) const;
		const VkSurfaceCapabilitiesKHR& GetCapabilities() const;


		uint32 GetFormatCount( ) const;
		const std::vector<VkSurfaceFormatKHR>& GetSurfaceFormats()const;

		VkSurfaceKHR GetSurface() { return m_Surface; }

	private:
		bool m_CanPresent = false;
		VkSurfaceKHR m_Surface = nullptr;

		VkSurfaceCapabilitiesKHR m_Capabilities;

		uint32 m_FormatCount = 0;
		std::vector<VkSurfaceFormatKHR> m_Formats;
		
		uint32 m_PresentCount = 0;
		std::vector<VkPresentModeKHR> m_PresentModes;

	};
}; // namespace Graphics