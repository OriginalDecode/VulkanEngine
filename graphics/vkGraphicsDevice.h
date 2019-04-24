#pragma once

#include "Core/utilities/utilities.h"
#include "Core/Defines.h"

#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

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

		std::vector<VkCommandBuffer> m_CmdBuffers;
		VkCommandPool m_CmdPool = nullptr;

		VkSemaphore m_AcquireNextImageSemaphore = nullptr;

		VkSemaphore m_DrawDone = nullptr;
		VkFence m_FrameFence[2]{ nullptr, nullptr };
		uint32 m_Index = 0;

		VkShaderModule LoadShader(const char* filepath, VkDevice pDevice);

    };

}; // namespace Graphics