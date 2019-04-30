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



        void DrawFrame(float dt);


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
		void CreateCube();
		VkVertexInputBindingDescription CreateBindDesc();
		VkVertexInputAttributeDescription CreateAttrDesc(int location, int offset);

		void CreateMatrixBuffer();


		void CreateBuffer(const VkBufferCreateInfo& createInfo, VkBuffer* buffer, VkDeviceMemory* memory);


		VkSemaphore CreateVkSemaphore(VkDevice pDevice);
		VkShaderModule LoadShader(const char* filepath, VkDevice pDevice);


    };


	class ConstantBuffer
	{
	private:
		struct Variable
		{
			Variable() = default;
			Variable(void* var, uint32 size) : var(var), size(size) {}
			void* var = nullptr;
			uint32 size = 0;
		};
	public:

		void Init(VkDevice logicDevice, VkPhysicalDevice physDevice);
		void Bind(VkDevice pDevice, int offset);

		void Destroy(VkDevice logicDevice)
		{
			vkDestroyBuffer(logicDevice, m_cBuffer, nullptr);
		}

		template<typename T>
		void RegVar(T* var);

		VkBuffer Get() const { return m_cBuffer; }

	private:
		uint32 m_BufferSize = 0;
		std::vector<Variable> m_Vars;
		VkBuffer m_cBuffer = nullptr;
		VkDeviceMemory m_DeviceMemory = nullptr;
	};

	template<typename T>
	void ConstantBuffer::RegVar(T* var)
	{
		m_Vars.push_back({ var, sizeof(T) });
		m_BufferSize += sizeof(T);
	}

}; // namespace Graphics