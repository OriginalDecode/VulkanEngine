#pragma once
#include "Core/Defines.h"
#include "Core/Types.h"
#include <Core/containers/GrowingArray.h>
#include "GraphicsDecl.h"

#include <vulkan/vulkan_core.h>

DEFINE_HANDLE(VkSurfaceKHR);
DEFINE_HANDLE(VkPhysicalDevice);
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

		void Init(VkSurfaceKHR pSurface, VlkPhysicalDevice* physicalDevice);

		bool CanPresent() const { return m_CanPresent; }

		const VkSurfaceCapabilitiesKHR& GetCapabilities() const;

		const Core::GrowingArray<VkSurfaceFormatKHR>& GetSurfaceFormats() const { return m_Formats; }

		const Core::GrowingArray<VkPresentModeKHR>& GetPresentModes() const { return m_PresentModes; }

		VkSurfaceKHR GetSurface() { return m_Surface; }

	private:
		bool m_CanPresent = false;
		VkSurfaceKHR m_Surface = nullptr;

		VkSurfaceCapabilitiesKHR m_Capabilities;

		Core::GrowingArray<VkSurfaceFormatKHR> m_Formats;

		Core::GrowingArray<VkPresentModeKHR> m_PresentModes;
	};
}; // namespace Graphics
