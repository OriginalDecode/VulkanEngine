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


		void SetupScissorArea();
		void SetupViewport();
		void CreateRenderPass();
		void CreateCommandPool();
		void CreateCommandBuffer();
		void CreateGraphicsPipeline();

		void CreateDescriptorLayout();
		void CreateDescriptorPool();
		void CreateDescriptorSet();

		void CreatePipelineLayout();
		void CreateFramebuffers();
		void CreateVertexBuffer(); 
		VkVertexInputBindingDescription CreateBindDesc();
		VkVertexInputAttributeDescription CreateAttrDesc();

		VkDeviceMemory GPUAllocateMemory(const VkMemoryRequirements& memRequirements);

		void CreateMatrixBuffer();


		VkSemaphore CreateVkSemaphore(VkDevice pDevice);
		VkShaderModule LoadShader(const char* filepath, VkDevice pDevice);

    };

}; // namespace Graphics