#pragma once

#include "Core/utilities/utilities.h"
#include "Core/Defines.h"

#include <vulkan/vulkan_core.h>

namespace Graphics
{

	struct RenderContextVulkan;
	class VulkanRenderer
	{
	public:
		VulkanRenderer() = default;
		~VulkanRenderer();

		bool Init(RenderContextVulkan* ctx);

		void DrawFrame(float dt);

	private:
		VkPipeline CreateGraphicsPipeline();
		void SetupRenderCommands(int index);
		void SetupRenderPass();
		void SetupDepthResources();
		void CreateImGuiContext();

		RenderContextVulkan* m_Context = nullptr;

		VkCommandPool m_CommandPool = nullptr;

		VkCommandBuffer* m_CommandsBuffers = nullptr;
		uint32 m_NofCommandBuffers{};

		VkFramebuffer* m_FrameBuffers = nullptr;
		uint32 m_NofFrameBuffers = {};

		VkDescriptorPool m_DescriptorPool = nullptr;
		VkRenderPass m_RenderPass = nullptr;

		VkSemaphore m_NextImage = nullptr;
		VkSemaphore m_DrawDone = nullptr;

		VkFence m_CommandFence = nullptr;
		VkFence m_FrameFence[2] = { nullptr, nullptr };
		uint32 m_Index{};

		VkViewport m_Viewport;
		VkRect2D m_ScissorArea;

		VkPipeline m_Pipeline = nullptr;
		VkPipelineLayout m_PipelineLayout = nullptr;

		VkDescriptorSet m_DescriptorSet = nullptr;
		VkDescriptorSetLayout m_DescriptorLayout = nullptr;

		VkImage m_DefaultDepthImage = nullptr;
		VkImageView m_DefaultDepthImageView = nullptr;
		VkDeviceMemory m_DepthMemory = nullptr;
		VkFormat m_DepthFormat;
	};

}; // namespace Graphics
