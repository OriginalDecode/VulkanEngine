#pragma once

#include "graphics/GraphicsDevice.h"

#include "Core/utilities/utilities.h"
#include "Core/Defines.h"

#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

class Window;

namespace Graphics
{

	typedef struct Shader HShader;
	class VulkanDevice;
	class ConstantBuffer;
	class VulkanRenderer final : public IGraphicsDevice
	{
	public:
		VulkanRenderer() = default;
		~VulkanRenderer();

		bool Init(const Window& window);

		void DrawFrame(float dt);

		virtual void BindConstantBuffer(ConstantBuffer* constantBuffer, uint32 offset) override;
		virtual void CreateConstantBuffer(ConstantBuffer* constantBuffer) override;
		virtual void DestroyConstantBuffer(ConstantBuffer* constantBuffer) override;

		void LoadShader(HShader* shader, const char* filepath);
		void DestroyShader(HShader* pShader);

	private:
		VulkanDevice* m_Device{ nullptr };
		VkCommandPool m_CommandPool{ nullptr };

		VkCommandBuffer* m_CommandsBuffers{ nullptr };
		uint32 m_NofCommandBuffers{ 0 };

		VkFramebuffer* m_FrameBuffers{ nullptr };
		uint32 m_NofFrameBuffers{ 0 };

		VkDescriptorPool m_DescriptorPool{ nullptr };
		VkRenderPass m_RenderPass{ nullptr };

		VkSemaphore m_NextImage = nullptr;
		VkSemaphore m_DrawDone = nullptr;

		VkFence m_CommandFence = nullptr;
		VkFence m_FrameFence[2]{ nullptr, nullptr };
		uint32 m_Index{ 0 };

		VkViewport m_Viewport = {};
		VkRect2D m_ScissorArea = {};

		VkPipeline m_Pipeline = nullptr;
		VkPipelineLayout m_PipelineLayout = nullptr;

		VkDescriptorSet m_DescriptorSet = nullptr;
		VkDescriptorSetLayout m_DescriptorLayout = nullptr;

		void SetupRenderCommands(int index);

		void CreateDescriptorSet();

		void SetupRenderPass();
		VkPipeline CreateGraphicsPipeline();

		void SetupDescriptorLayout(VkDescriptorSetLayoutBinding* descriptorBindings,
								   int32 bindingCount);

		VkPipelineLayout CreatePipelineLayout(VkDescriptorSetLayout* descriptorLayouts,
											  int32 descriptorLayoutCount,
											  VkPushConstantRange* pushConstantRange,
											  int32 pushConstantRangeCount);

		VkVertexInputBindingDescription CreateBindDesc();
		VkVertexInputAttributeDescription CreateAttrDesc(int location, int offset);

		VkShaderModule LoadShader(const char* filepath, VkDevice pDevice);

		VkPipelineShaderStageCreateInfo CreateShaderStageInfo(VkShaderStageFlagBits stageFlags,
															  VkShaderModule module,
															  const char* entryPoint);
	};

}; // namespace Graphics
