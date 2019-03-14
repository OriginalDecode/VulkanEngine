#pragma once
#include "Core/utilities/utilities.h"
#include "Core/Defines.h"
#include <vector>
#include <vulkan/vulkan_core.h>
#include <memory>
class Window;
namespace Graphics
{
    class VlkInstance;
    class VlkPhysicalDevice;
    class VlkDevice;
	class VlkSwapchain;
    class vkGraphicsDevice
    {
    public:
        vkGraphicsDevice() = default;
        ~vkGraphicsDevice();

        bool Init( const Window& window );

        void Present();
        void DrawFrame();

    private:
        void CreateSwapchain( const Window& window );
        VkSurfaceKHR CreateSurface( HWindow windowHandle );
        void CreateFramebuffers( const Window& window );
        VkRenderPass CreateRenderPass();
        void CreateImageViews();
        void CreateCommandPool();
        void CreateCommandBuffers();
        void CreateSemaphores();

        std::unique_ptr<VlkInstance> m_Instance;
        std::unique_ptr<VlkPhysicalDevice> m_PhysicalDevice;
        std::unique_ptr<VlkDevice> m_LogicalDevice;
        std::unique_ptr<VlkSwapchain> m_Swapchain;

        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;
        std::vector<VkFramebuffer> m_FrameBuffers;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        uint32 m_Format;
        VkSurfaceKHR m_Backbuffer = nullptr;

        VkRenderPass m_RenderPass = nullptr;
        VkCommandPool m_CommandPool = nullptr;

        VkSemaphore m_RendererFinished = nullptr;
        VkSemaphore m_ImageAvailable = nullptr;

        VkExtent2D m_Extent;
    };

}; // namespace Graphics