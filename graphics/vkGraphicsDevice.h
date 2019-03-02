#pragma once
#include "Core/utilities/utilities.h"

LPE_DEFINE_HANDLE( VkInstance );
LPE_DEFINE_HANDLE( VkDevice );
LPE_DEFINE_HANDLE( VkPhysicalDevice );
LPE_DEFINE_HANDLE( VkQueue );

namespace Graphics
{
    class vkGraphicsDevice
    {
    public:
        vkGraphicsDevice();
        ~vkGraphicsDevice();

    private:
        void CreateInstance();
        void CreateDevice();
        void CreateSwapchain();

        VkQueue m_Queue = nullptr;
        VkDevice m_Device = nullptr;
        VkInstance m_Instance = nullptr;
        VkPhysicalDevice m_PhysicalDevice = nullptr;
    };

}; // namespace Graphics