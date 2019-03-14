#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>

#include "IGfxObject.h"


#include <vulkan/vulkan_core.h>

LPE_DEFINE_HANDLE( VkPhysicalDevice );
LPE_DEFINE_HANDLE( VkDevice );

struct VkDeviceCreateInfo;
namespace Graphics
{
    class VlkInstance;
	class VlkSurface;
    class VlkPhysicalDevice
    {
    public:
        VlkPhysicalDevice() = default;
        ~VlkPhysicalDevice();

        void Init( const VlkInstance& instance );

        uint32 GetQueueFamilyIndex() const { return m_QueueFamilyIndex; }

		VkDevice CreateDevice( const VkDeviceCreateInfo& createInfo ) const;

		bool SurfaceCanPresent( VkSurfaceKHR pSurface ) const;
		uint32 GetSurfacePresentModeCount( const VlkSurface& surface ) const;
		void GetSurfacePresentModes( const VlkSurface& surface, uint32 modeCount, VkPresentModeKHR* presentModes ) const;
		VkSurfaceCapabilitiesKHR GetSurfaceCapabilities( const VlkSurface& surface ) const;
    private:
        VkPhysicalDevice m_PhysicalDevice = nullptr;
        uint32 m_QueueFamilyIndex = 0;
		std::vector<VkQueueFamilyProperties> m_QueueProperties;
    };

}; //namespace Graphics