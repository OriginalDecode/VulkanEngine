#include "VulkanSurface.h"
#include "logger/Debug.h"

namespace Graphics
{
	VulkanSurface::VulkanSurface() = default;
	VulkanSurface::~VulkanSurface() { ASSERT( m_Surface, "Surface haven't been removed!" ); }

	void VulkanSurface::Init( VkSurfaceKHR pSurface, VlkPhysicalDevice* physicalDevice )
	{
		m_Surface = pSurface;
		physicalDevice->GetSurfaceInfo( pSurface, &m_CanPresent, &m_FormatCount, &m_Formats, &m_PresentCount,
										&m_PresentModes, &m_Capabilities );
	}

	bool VulkanSurface::CanPresent() const { return m_CanPresent; }

	/*int32 VulkanSurface::GetPresentModeCount( VkPhysicalDevice pPhysicalDevice ) const
	{
		uint32 out = 0;
		VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR( pPhysicalDevice, m_Surface, &out, nullptr);
		assert( result == VK_SUCCESS );
		return out;
	}*/

	// uint32 VulkanSurface::GetFormatCount() const
	//{

	//	return m_FormatCount;
	//}

	const std::vector<VkSurfaceFormatKHR>& VulkanSurface::GetSurfaceFormats() const { return m_Formats; }

	const VkSurfaceCapabilitiesKHR& VulkanSurface::GetCapabilities() const { return m_Capabilities; }

}; // namespace Graphics
