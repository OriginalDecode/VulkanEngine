#pragma once

#include "GraphicsDevice.h"

#include "Core/utilities/utilities.h"
#include "Core/Defines.h"

#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

class Window;
namespace Graphics
{

	typedef struct Shader HShader;

	class ConstantBuffer;
	class VlkInstance;
	class VlkPhysicalDevice;
	class VlkDevice;
	class VlkSwapchain;

	class vkGraphicsDevice final : public IGraphicsDevice
	{
	public:
		vkGraphicsDevice();
		~vkGraphicsDevice();

		class Camera* GetCamera();
		bool Init(const Window& window);

		void DrawFrame(float dt);

		VlkInstance& GetVlkInstance()
		{
			return *m_Instance;
		}
		VlkDevice& GetVlkDevice()
		{
			return *m_LogicalDevice;
		}

		virtual void BindConstantBuffer(ConstantBuffer* constantBuffer, uint32 offset) override;
		virtual void CreateConstantBuffer(ConstantBuffer* constantBuffer) override;
		virtual void DestroyConstantBuffer(ConstantBuffer* constantBuffer) override;

		void LoadShader(HShader* shader, const char* filepath);
		void DestroyShader(HShader* pShader);

	private:
		VlkInstance* m_Instance = nullptr;
		VlkPhysicalDevice* m_PhysicalDevice = nullptr;
		VlkDevice* m_LogicalDevice = nullptr;
		VlkSwapchain* m_Swapchain = nullptr;

		std::vector<VkCommandBuffer> m_CmdBuffers;
		VkCommandPool m_CmdPool = nullptr;

		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat findDepthFormat();

		VkSemaphore m_AcquireNextImageSemaphore = nullptr;

		VkSemaphore m_DrawDone = nullptr;
		VkFence m_CommandFence = nullptr;
		VkFence m_FrameFence[2]{ nullptr, nullptr };
		uint32 m_Index = 0;
		std::vector<VkFramebuffer> m_FrameBuffers;

		VkRenderPass CreateRenderPass();
		void CreateCommandPool();
		VkCommandBuffer CreateCommandBuffer(VkDevice device, VkCommandPool pool, VkCommandBufferLevel bufferLevel);
		VkPipeline CreateGraphicsPipeline();

		VkDescriptorSetLayout CreateDescriptorLayout(VkDescriptorSetLayoutBinding* descriptorBindings, int32 bindingCount);
		void CreateDescriptorPool();
		void CreateDescriptorSet();

		VkPipelineLayout CreatePipelineLayout(VkDescriptorSetLayout* descriptorLayouts, int32 descriptorLayoutCount, VkPushConstantRange* pushConstantRange,
											  int32 pushConstantRangeCount);
		VkImageView CreateImageView(VkFormat format, VkImage image, VkImageAspectFlags aspectFlag);
		VkFramebuffer CreateFramebuffer(VkImageView* view, int32 attachmentCount, const Window& window);

		void CreateImage(uint32 width, uint32 height, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image,
						 VkDeviceMemory& imageMemory);

		VkVertexInputBindingDescription CreateBindDesc();
		VkVertexInputAttributeDescription CreateAttrDesc(int location, int offset);

		void CreateDepthResources();

		VkSemaphore CreateVkSemaphore(VkDevice pDevice);
		VkShaderModule LoadShader(const char* filepath, VkDevice pDevice);

		// rewrite
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		// rewrite

		VkPipelineShaderStageCreateInfo CreateShaderStageInfo(VkShaderStageFlagBits stageFlags, VkShaderModule module, const char* entryPoint);

		void CreateViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth, VkViewport* viewport);

		void SetupScissorArea(uint32 width, uint32 height, int32 offsetX, int32 offsetY, VkRect2D* scissorArea);

		void SetupImGui();

		void SetupRenderCommands(int index);
		void PrepareRenderPass(VkRenderPassBeginInfo* pass_info, VkFramebuffer framebuffer, uint32 width, uint32 height);
	};

	class ConstantBuffer
	{
	public:
		struct Variable
		{
			Variable() = default;
			Variable(void* var, uint32 size)
				: var(var)
				, size(size)
			{
			}
			void* var = nullptr;
			uint32 size = 0;
		};

		void SetBuffer(void* buffer)
		{
			m_Buffer = buffer;
		}
		void SetDeviceMemory(void* memory)
		{
			m_DeviceMemory = memory;
		}

		template <typename T>
		void RegVar(T* var);

		void* GetBuffer()
		{
			return m_Buffer;
		}
		void* GetDeviceMemory()
		{
			return m_DeviceMemory;
		}
		uint32 GetSize() const
		{
			return m_BufferSize;
		}
		void* GetData()
		{
			return m_Vars.data();
		}
		const std::vector<Variable>& GetVariables() const
		{
			return m_Vars;
		}

	private:
		uint32 m_BufferSize = 0;
		std::vector<Variable> m_Vars;
		void* m_Buffer = nullptr;
		void* m_DeviceMemory = nullptr;
	};

	template <typename T>
	void ConstantBuffer::RegVar(T* var)
	{
		m_Vars.push_back({ var, sizeof(T) });
		m_BufferSize += sizeof(T);
	}

}; // namespace Graphics
