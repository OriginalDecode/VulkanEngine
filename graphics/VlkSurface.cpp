#include "VlkSurface.h"

#include "VlkInstance.h"
#include "vkGraphicsDevice.h"
#include "VlkPhysicalDevice.h"
#include "GraphicsEngine.h"

#include <cassert>

namespace Graphics
{
	VlkSurface::~VlkSurface()
	{
		Release();
	}

	void VlkSurface::Release()
	{
		GraphicsEngine::GetDevice().GetVlkInstance().DestroySurface( m_Surface );
	}

	void VlkSurface::Init( VkSurfaceKHR pSurface, const VlkPhysicalDevice& physicalDevice )
	{
		m_Surface = pSurface;

		m_CanPresent = physicalDevice.SurfaceCanPresent( m_Surface );
	}

	bool VlkSurface::CanPresent() const
	{
		return m_CanPresent;
	}

	uint32 VlkSurface::GetPresentModeCount( VkPhysicalDevice pPhysicalDevice ) const
	{
		uint32 out = 0;
		VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR( pPhysicalDevice, m_Surface, &out, nullptr);
		assert( result == VK_SUCCESS );
		return out;
	}

	void VlkSurface::GetPresentModes( VkPhysicalDevice pPhysicalDevice, uint32 modeCount, VkPresentModeKHR* presentModes ) const
	{
		VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR( pPhysicalDevice, m_Surface, &modeCount, presentModes );
		assert( result == VK_SUCCESS );
	}

	uint32 VlkSurface::GetFormatCount( VkPhysicalDevice pPhysicalDevice ) const
	{
		uint32 formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR( pPhysicalDevice, m_Surface, &formatCount, nullptr );

	}

	void VlkSurface::GetSurfaceFormat( VkPhysicalDevice pPhysicalDevice, VkSurfaceFormatKHR* formats )
	{
		


	}

	VkSurfaceCapabilitiesKHR VlkSurface::GetCapabilities( const VlkPhysicalDevice& physicalDevice ) const
	{
		return physicalDevice.GetSurfaceCapabilities( m_Surface );
	}

}; // namespace Graphics
