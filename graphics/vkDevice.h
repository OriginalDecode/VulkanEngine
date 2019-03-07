#pragma once
#include <Core/Define.h>

LPE_DEFINE_HANDLE( VkDevice );
LPE_DEFINE_HANDLE( VkQueue );

namespace Graphics
{
    class vkPhysicalDevice;
    class vkDevice
    {
    public:
        vkDevice() = default;
        ~vkDevice();

        void Init( const vkPhysicalDevice& physicalDevice );
        void Release();

        VkDevice GetDevice() const { return m_Device; }
        VkQueue GetQueue() const { return m_Queue; }

    private:
        VkDevice m_Device = nullptr;
        VkQueue m_Queue = nullptr;
    };

}; //namespace Graphics