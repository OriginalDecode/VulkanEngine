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

	void VlkSurface::Init( VkSurfaceKHR pSurface )
	{
		m_Surface = pSurface;
	}

	bool VlkSurface::CanPresent( const VlkPhysicalDevice& physicalDevice ) const
	{
		return physicalDevice.SurfaceCanPresent( m_Surface );
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

	VkSurfaceCapabilitiesKHR VlkSurface::GetCapabilities(VkPhysicalDevice pPhysicalDevice) const
	{
		VkSurfaceCapabilitiesKHR capabilities = { };
		VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR( pPhysicalDevice, m_Surface, &capabilities );
		assert( result == VK_SUCCESS );
		return capabilities;

	}

}; // namespace Graphics
