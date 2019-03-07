#pragma once
#include <Core/Defines.h>
#include <Core/Types.h>

LPE_DEFINE_HANDLE( VkPhysicalDevice );

namespace Graphics
{
    class vkInstance;
    class vkPhysicalDevice
    {
    public:
        vkPhysicalDevice() = default;
        ~vkPhysicalDevice();

        void Init( const vkInstance& instance );

        uint32 GetQueueFamilyIndex() const { return m_QueueFamilyIndex; }

    private:
        VkPhysicalDevice m_PhysicalDevice = nullptr;
        uint32 m_QueueFamilyIndex = 0;
    };

}; //namespace Graphics