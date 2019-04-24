#include "vkGraphicsDevice.h"

#include "VlkInstance.h"
#include "VlkPhysicalDevice.h"
#include "VlkDevice.h"
#include "VlkSwapchain.h"

#include "Utilities.h"
#include "Window.h"

#include <cassert>
#include <windows.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#include "Core/File.h"

VkClearColorValue _clearColor = { 0.f, 0.f, 0.f, 0.f };

VkRenderPass _renderPass = nullptr;

std::vector<VkFramebuffer> m_FrameBuffers;
VkShaderModule vertModule = nullptr;
VkShaderModule fragModule = nullptr;
VkPipeline _pipeline = nullptr;
VkPipelineLayout _pipelineLayout = nullptr;
VkViewport _Viewport = {};
VkRect2D _Scissor = {};
Window::Size _size;

namespace Graphics
{
	vkGraphicsDevice::vkGraphicsDevice() = default;

	vkGraphicsDevice::~vkGraphicsDevice() = default;

	bool vkGraphicsDevice::Init(const Window& window)
	{
		_size = window.GetInnerSize();
		m_Instance = std::make_unique<VlkInstance>();
		m_Instance->Init();

		m_PhysicalDevice = std::make_unique<VlkPhysicalDevice>();
		m_PhysicalDevice->Init(m_Instance.get());

		m_LogicalDevice = std::make_unique<VlkDevice>();
		m_LogicalDevice->Init(m_PhysicalDevice.get());

		m_Swapchain = std::make_unique<VlkSwapchain>();
		m_Swapchain->Init(m_Instance.get(), m_LogicalDevice.get(), m_PhysicalDevice.get(), window);

		VkCommandPoolCreateInfo poolCreateInfo = {};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolCreateInfo.pNext = nullptr;
		poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolCreateInfo.queueFamilyIndex = m_PhysicalDevice->GetQueueFamilyIndex();

		if (vkCreateCommandPool(m_LogicalDevice->GetDevice(), &poolCreateInfo, nullptr, &m_CmdPool) != VK_SUCCESS)
			assert(!"failed to create commandpool!");

		VkCommandBufferAllocateInfo cmdBufAllocInfo = {};
		cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocInfo.commandPool = m_CmdPool;
		cmdBufAllocInfo.commandBufferCount = (uint32)m_Swapchain->GetNofImages();
		cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		m_CmdBuffers.resize(m_Swapchain->GetNofImages());
		if (vkAllocateCommandBuffers(m_LogicalDevice->GetDevice(), &cmdBufAllocInfo, m_CmdBuffers.data()) != VK_SUCCESS)
			assert(!"Failed to create command buffer!");

		VkAttachmentReference attRef = {};
		attRef.attachment = 0;
		attRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDesc = {};
		subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDesc.colorAttachmentCount = 1;
		subpassDesc.pColorAttachments = &attRef;

		VkFormat format = m_Swapchain->GetFormat().format;

		VkAttachmentDescription attDesc = {};
		attDesc.format = format;
		attDesc.samples = VK_SAMPLE_COUNT_1_BIT;

		attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		attDesc.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		attDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkRenderPassCreateInfo rpInfo = {};
		rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		rpInfo.attachmentCount = 1;
		rpInfo.pAttachments = &attDesc;
		rpInfo.subpassCount = 1;
		rpInfo.pSubpasses = &subpassDesc;

		if (vkCreateRenderPass(m_LogicalDevice->GetDevice(), &rpInfo, nullptr, &_renderPass) != VK_SUCCESS)
			assert(!"Failed to create renderpass");

	
		//assert( !"Failed to create render pass!" );

		m_FrameBuffers.resize(m_Swapchain->GetNofImages());
		auto& list = m_Swapchain->GetImageList();
		auto& viewList = m_Swapchain->GetImageViewList();
		for (size_t i = 0; i < m_Swapchain->GetNofImages(); ++i)
		{
			VkImageViewCreateInfo vcInfo = {};
			vcInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			vcInfo.image = list[i];
			vcInfo.format = format;
			vcInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			vcInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY,
								  VK_COMPONENT_SWIZZLE_IDENTITY,
								  VK_COMPONENT_SWIZZLE_IDENTITY,
								  VK_COMPONENT_SWIZZLE_IDENTITY };
			VkImageSubresourceRange& srr = vcInfo.subresourceRange;
			srr.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			srr.baseMipLevel = 0;
			srr.levelCount = 1;
			srr.baseArrayLayer = 0;
			srr.layerCount = 1;

			if (vkCreateImageView(m_LogicalDevice->GetDevice(), &vcInfo, nullptr, &viewList[i]) != VK_SUCCESS)
				assert(!"Failed to create imageview!");

			VkFramebufferCreateInfo fbInfo = {};
			fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			fbInfo.renderPass = _renderPass;
			fbInfo.attachmentCount = 1;
			fbInfo.pAttachments = &viewList[i];
			fbInfo.width = (uint32)_size.m_Width;
			fbInfo.height = (uint32)_size.m_Height;
			fbInfo.layers = 1;

			if (vkCreateFramebuffer(m_LogicalDevice->GetDevice(), &fbInfo, nullptr, &m_FrameBuffers[i]) != VK_SUCCESS)
				assert(!"Failed to create framebuffer!");
		}

		vertModule = LoadShader("Data/Shaders/vertex.vert", m_LogicalDevice->GetDevice());
		fragModule = LoadShader("Data/Shaders/frag.frag", m_LogicalDevice->GetDevice());


		VkPipelineShaderStageCreateInfo vertexStageInfo = {};
		vertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexStageInfo.module = vertModule;
		vertexStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragStageInfo = {};
		fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragStageInfo.module = fragModule;
		fragStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo ssci[] = { vertexStageInfo, fragStageInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;



		VkPipelineInputAssemblyStateCreateInfo pipelineIACreateInfo = {};
		pipelineIACreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipelineIACreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipelineIACreateInfo.primitiveRestartEnable = VK_FALSE;

		_Viewport.x = 0.0f;
		_Viewport.y = 0.0f;
		_Viewport.width = _size.m_Width;
		_Viewport.height = _size.m_Height;
		_Viewport.minDepth = 0.0f;
		_Viewport.maxDepth = 1.0f;

		
		_Scissor.extent.width = (uint32)_size.m_Width;
		_Scissor.extent.height = (uint32)_size.m_Height;
		_Scissor.offset.x = 0;
		_Scissor.offset.y = 0;

		VkPipelineViewportStateCreateInfo vpCreateInfo = {};
		vpCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		vpCreateInfo.viewportCount = 1;
		vpCreateInfo.scissorCount = 1;
		vpCreateInfo.pScissors = &_Scissor;
		vpCreateInfo.pViewports = &_Viewport;

		VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
		pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
		pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
		pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_ALWAYS;
		pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
		pipelineDepthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
		pipelineDepthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
		pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
		pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
		pipelineDepthStencilStateCreateInfo.front = pipelineDepthStencilStateCreateInfo.back;

		VkPipelineRasterizationStateCreateInfo rastCreateInfo = {};
		rastCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rastCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rastCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rastCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rastCreateInfo.depthClampEnable = VK_FALSE;
		rastCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rastCreateInfo.depthBiasEnable = VK_FALSE;
		rastCreateInfo.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo pipelineMSCreateInfo = {};
		pipelineMSCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipelineMSCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pipelineMSCreateInfo.sampleShadingEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState blendAttachState = {};
		blendAttachState.colorWriteMask = 0xF;
		blendAttachState.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo blendCreateInfo = {};
		blendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		blendCreateInfo.logicOpEnable = VK_FALSE;
		blendCreateInfo.attachmentCount = 1;
		blendCreateInfo.pAttachments = &blendAttachState;




		VkPipelineLayoutCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		if (vkCreatePipelineLayout(m_LogicalDevice->GetDevice(), &pipelineCreateInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
			assert(!"Failed to create pipelineLayout");

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.layout = _pipelineLayout;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &pipelineIACreateInfo;
		pipelineInfo.renderPass = _renderPass;
		pipelineInfo.pViewportState = &vpCreateInfo;
		pipelineInfo.pColorBlendState = &blendCreateInfo;
		pipelineInfo.pRasterizationState = &rastCreateInfo;
		pipelineInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
		pipelineInfo.pMultisampleState = &pipelineMSCreateInfo;
		pipelineInfo.pStages = ssci;
		pipelineInfo.stageCount = ARRSIZE( ssci );

		if (vkCreateGraphicsPipelines(m_LogicalDevice->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline) != VK_SUCCESS)
			assert(!"Failed to create pipeline!");


		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		if (vkCreateSemaphore(m_LogicalDevice->GetDevice(), &semaphoreCreateInfo, nullptr, &m_AcquireNextImageSemaphore) != VK_SUCCESS)
			assert(!"Failed to create semaphore");

		if (vkCreateSemaphore(m_LogicalDevice->GetDevice(), &semaphoreCreateInfo, nullptr, &m_DrawDone) != VK_SUCCESS)
			assert(!"Failed to create semaphore");


		VkImageSubresourceRange imageRange = {};
		imageRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageRange.levelCount = 1;
		imageRange.layerCount = 1;


		return true;
	}

	void vkGraphicsDevice::Present()
	{
	}

	void vkGraphicsDevice::DrawFrame()
	{


		VkCommandBufferBeginInfo cmdInfo = {};
		cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		VkClearValue clearValue = {};
		clearValue.color = _clearColor;

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _renderPass;
		renderPassInfo.renderArea.offset.x = 0;
		renderPassInfo.renderArea.offset.y = 0;
		renderPassInfo.renderArea.extent.width = (uint32)_size.m_Width;
		renderPassInfo.renderArea.extent.height = (uint32)_size.m_Height;
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearValue;

		for (size_t i = 0; i < m_CmdBuffers.size(); ++i)
		{
			if (vkBeginCommandBuffer(m_CmdBuffers[i], &cmdInfo) != VK_SUCCESS)
				assert(!"Failed to begin commandbuffer!");
			renderPassInfo.framebuffer = m_FrameBuffers[i];

			vkCmdBeginRenderPass(m_CmdBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


			vkCmdBindPipeline(m_CmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

			vkCmdSetViewport(m_CmdBuffers[i], 0, 1, &_Viewport);
			vkCmdSetScissor(m_CmdBuffers[i], 0, 1, &_Scissor);

			vkCmdDraw(m_CmdBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(m_CmdBuffers[i]);

			if (vkEndCommandBuffer(m_CmdBuffers[i]) != VK_SUCCESS)
				assert(!"Failed to end commandbuffer!");
		}

		if (vkAcquireNextImageKHR(m_LogicalDevice->GetDevice(), m_Swapchain->GetSwapchain(), UINT64_MAX, m_AcquireNextImageSemaphore, VK_NULL_HANDLE/*fence*/, &m_Index) != VK_SUCCESS)
			assert(!"Failed to acquire next image!");

		const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //associated with having semaphores
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitDstStageMask = &waitDstStageMask;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CmdBuffers[m_Index];

		submitInfo.pSignalSemaphores = &m_DrawDone;
		submitInfo.signalSemaphoreCount = 1;

		submitInfo.pWaitSemaphores = &m_AcquireNextImageSemaphore;
		submitInfo.waitSemaphoreCount = 1;

		if (vkQueueSubmit(m_LogicalDevice->GetQueue(), 1, &submitInfo, nullptr) != VK_SUCCESS)
			assert(!"Failed to submit the queue!");

		VkSwapchainKHR swapchain = m_Swapchain->GetSwapchain();

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &m_Index;
		presentInfo.pWaitSemaphores = &m_DrawDone;
		presentInfo.waitSemaphoreCount = 1;

		if (vkQueuePresentKHR(m_LogicalDevice->GetQueue(), &presentInfo) != VK_SUCCESS)
			assert(!"Failed to present!");
	}


	VkShaderModule vkGraphicsDevice::LoadShader(const char* filepath, VkDevice pDevice)
	{
		Core::File shader(filepath, Core::FileMode::READ_FILE);
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shader.GetSize();
		createInfo.pCode = (const uint32_t*)shader.GetBuffer();

		VkShaderModule shaderModule = nullptr;
		if (vkCreateShaderModule(pDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			assert(!"Failed to create shader module!");

		return shaderModule;
	}

}; //namespace Graphics