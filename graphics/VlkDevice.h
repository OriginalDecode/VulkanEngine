#pragma once
#include <Core/Defines.h>
#include "IGfxDevice.h"
LPE_DEFINE_HANDLE( VkDevice );
LPE_DEFINE_HANDLE( VkQueue );
LPE_DEFINE_HANDLE( VkSwapchainKHR );

struct VkSwapchainCreateInfoKHR;
namespace Graphics
{
    class VlkPhysicalDevice;
    class VlkDevice : public IGfxDevice
    {
    public:
        VlkDevice() = default;
        ~VlkDevice();

        void Init( const VlkPhysicalDevice& physicalDevice );
        void Release(IGfxDevice* device) override;

        VkDevice GetDevice() const { return m_Device; }
        VkQueue GetQueue() const { return m_Queue; }

		VkSwapchainKHR CreateSwapchain(const VkSwapchainCreateInfoKHR& createInfo ) const;

    private:
        VkDevice m_Device = nullptr;
        VkQueue m_Queue = nullptr;
    };

}; //namespace Graphics