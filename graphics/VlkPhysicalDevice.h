#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>

#include "IGfxObject.h"

LPE_DEFINE_HANDLE( VkPhysicalDevice );
LPE_DEFINE_HANDLE( VkDevice );

struct VkDeviceCreateInfo;
namespace Graphics
{
    class VlkInstance;
    class VlkPhysicalDevice
    {
    public:
        VlkPhysicalDevice() = default;
        ~VlkPhysicalDevice();

        void Init( const VlkInstance& instance );

        uint32 GetQueueFamilyIndex() const { return m_QueueFamilyIndex; }

		VkDevice CreateDevice( const VkDeviceCreateInfo& createInfo ) const;

    private:
        VkPhysicalDevice m_PhysicalDevice = nullptr;
        uint32 m_QueueFamilyIndex = 0;
		std::vector<VkQueueFamilyProperties> m_QueueProperties;
    };

}; //namespace Graphics