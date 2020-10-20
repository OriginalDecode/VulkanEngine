#include "VlkSurface.h"

#include "VlkInstance.h"
#include "vkGraphicsDevice.h"
#include "VlkPhysicalDevice.h"
#include "GraphicsEngine.h"

#include <cassert>

namespace Graphics
{
	VlkSurface::VlkSurface() = default;
	VlkSurface::~VlkSurface() { Release(); }

	void VlkSurface::Release() { GraphicsEngine::GetDevice().GetVlkInstance().DestroySurface(m_Surface); }

	void VlkSurface::Init(VkSurfaceKHR pSurface, VlkPhysicalDevice* physicalDevice)
	{
		m_Surface = pSurface;
		physicalDevice->GetSurfaceInfo(pSurface, &m_CanPresent, m_Formats, m_PresentModes, &m_Capabilities);
	}

	const VkSurfaceCapabilitiesKHR& VlkSurface::GetCapabilities() const { return m_Capabilities; }

}; // namespace Graphics
