#pragma once
#include "Core/utilities/utilities.h"
#include "Core/Defines.h"
#include <vector>
#include <vulkan/vulkan_core.h>

class Window;
namespace Graphics
{
    class vkGraphicsDevice
    {
    public:
        vkGraphicsDevice();
        ~vkGraphicsDevice();

        bool Init( const Window* window );

        void Present();

    private:
        VkInstance CreateInstance();
        bool GetPhysicalDevice( uint32_t* outDeviceIndex, VkPhysicalDevice* physicalDevice, VkInstance instance );
        bool CreateDeviceAndQueue();
        void CreateSwapchain( const Window* window );
        void SetupDebugCallback();
        VkSurfaceKHR CreateSurface( HWindow windowHandle );
        void CreateFramebuffers( const Window* window );
        VkRenderPass CreateRenderPass();
        void CreateImageViews();
        void CreateCommandPool();
        void CreateCommandBuffers();
        void CreatePipeline();

        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;
        std::vector<VkFramebuffer> m_FrameBuffers;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        uint32 m_Format;
        VkQueue m_Queue = nullptr;
        VkDevice m_Device = nullptr;
        VkInstance m_Instance = nullptr;
        VkSurfaceKHR m_Backbuffer = nullptr;
        VkRenderPass m_RenderPass = nullptr;
        VkSwapchainKHR m_Swapchain = nullptr;
        VkPhysicalDevice m_PhysicalDevice = nullptr;
        VkCommandPool m_CommandPool = nullptr;

        VkExtent2D m_Extent;
    };

}; // namespace Graphics