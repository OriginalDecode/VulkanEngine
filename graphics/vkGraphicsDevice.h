#pragma once

#include "Core/utilities/utilities.h"
#include "Core/Defines.h"

#include <memory>

class Window;
//struct VkWin32SurfaceCreateInfoKHR;
namespace Graphics
{
    class VlkInstance;
    class VlkPhysicalDevice;
    class VlkDevice;
	class VlkSwapchain;
    class vkGraphicsDevice
    {
    public:
        vkGraphicsDevice();
        ~vkGraphicsDevice();

        bool Init( const Window& window );

        void Present();
        void DrawFrame();


		VlkInstance& GetVlkInstance() { return *m_Instance; }
		VlkDevice& GetVlkDevice() { return *m_LogicalDevice; }

    private:
  
        std::unique_ptr<VlkInstance> m_Instance;
        std::unique_ptr<VlkPhysicalDevice> m_PhysicalDevice;
        std::unique_ptr<VlkDevice> m_LogicalDevice;
        std::unique_ptr<VlkSwapchain> m_Swapchain;

        //std::vector<VkImage> m_Images;
        //std::vector<VkImageView> m_ImageViews;
        //std::vector<VkFramebuffer> m_FrameBuffers;
        //std::vector<VkCommandBuffer> m_CommandBuffers;

       /* uint32 m_Format;
        VkSurfaceKHR m_Backbuffer = nullptr;

        VkRenderPass m_RenderPass = nullptr;
        VkCommandPool m_CommandPool = nullptr;

        VkSemaphore m_RendererFinished = nullptr;
        VkSemaphore m_ImageAvailable = nullptr;

        VkExtent2D m_Extent;*/
    };

}; // namespace Graphics