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
Window::Size _size;
namespace Graphics
{
	vkGraphicsDevice::vkGraphicsDevice() = default;

	vkGraphicsDevice::~vkGraphicsDevice() = default;

	bool vkGraphicsDevice::Init( const Window& window )
	{
		_size = window.GetSize();
		m_Instance = std::make_unique<VlkInstance>();
		m_Instance->Init();

		m_PhysicalDevice = std::make_unique<VlkPhysicalDevice>();
		m_PhysicalDevice->Init( m_Instance.get() );

		m_LogicalDevice = std::make_unique<VlkDevice>();
		m_LogicalDevice->Init( m_PhysicalDevice.get() );

		m_Swapchain = std::make_unique<VlkSwapchain>();
		m_Swapchain->Init( m_Instance.get(), m_LogicalDevice.get(), m_PhysicalDevice.get(), window );

		VkCommandPoolCreateInfo poolCreateInfo = {};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolCreateInfo.queueFamilyIndex = m_PhysicalDevice->GetQueueFamilyIndex();
		if( vkCreateCommandPool( m_LogicalDevice->GetDevice(), &poolCreateInfo, nullptr, &m_CmdPool ) != VK_SUCCESS )
			assert( !"failed to create commandpool!" );

		VkCommandBufferAllocateInfo cmdBufAllocInfo = {};
		cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocInfo.commandPool = m_CmdPool;
		cmdBufAllocInfo.commandBufferCount = (uint32)m_Swapchain->GetNofImages();
		cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		m_CmdBuffers.resize( m_Swapchain->GetNofImages() );
		if( vkAllocateCommandBuffers( m_LogicalDevice->GetDevice(), &cmdBufAllocInfo, m_CmdBuffers.data() ) != VK_SUCCESS )
			assert( !"Failed to create command buffer!" );

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
		attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attDesc.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		attDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkRenderPassCreateInfo rpInfo = {};
		rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		rpInfo.attachmentCount = 1;
		rpInfo.pAttachments = &attDesc;
		rpInfo.subpassCount = 1;
		rpInfo.pSubpasses = &subpassDesc;

		if( vkCreateRenderPass( m_LogicalDevice->GetDevice(), &rpInfo, nullptr, &_renderPass ) != VK_SUCCESS )
			assert( !"Failed to create render pass!" );

		m_FrameBuffers.resize( m_Swapchain->GetNofImages() );
		auto& list = m_Swapchain->GetImageList();
		auto& viewList = m_Swapchain->GetImageViewList();
		for( size_t i = 0; i < m_Swapchain->GetNofImages(); ++i )
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

			if( vkCreateImageView( m_LogicalDevice->GetDevice(), &vcInfo, nullptr, &viewList[i] ) != VK_SUCCESS )
				assert( !"Failed to create imageview!" );

			VkFramebufferCreateInfo fbInfo = {};
			fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			fbInfo.renderPass = _renderPass;
			fbInfo.attachmentCount = 1;
			fbInfo.pAttachments = &viewList[i];
			fbInfo.width = (uint32)window.GetSize().m_Width;
			fbInfo.height = (uint32)window.GetSize().m_Height;
			fbInfo.layers = 1;

			if( vkCreateFramebuffer( m_LogicalDevice->GetDevice(), &fbInfo, nullptr, &m_FrameBuffers[i] ) != VK_SUCCESS )
				assert( !"Failed to create framebuffer!" );

			//int byteSize = 0;
			//char* shader = nullptr;

			Core::File frag( "Data/Shaders/frag.frag" ); //the file type should probably take read flags or write flags?
			VkShaderModuleCreateInfo fscInfo = {};
			fscInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			fscInfo.codeSize = frag.GetSize();
			fscInfo.pCode = (const uint32_t*)frag.GetBuffer();

			if( vkCreateShaderModule( m_LogicalDevice->GetDevice(), &fscInfo, nullptr, &fragModule ) != VK_SUCCESS )
				assert( !"Failed to create shader module!" );

			Core::File vert( "Data/shaders/vertex.vert" ); //the file type should probably take read flags or write flags?
			VkShaderModuleCreateInfo vscInfo = {};
			vscInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			vscInfo.codeSize = vert.GetSize();
			vscInfo.pCode = (const uint32_t*)vert.GetBuffer();

			if( vkCreateShaderModule( m_LogicalDevice->GetDevice(), &vscInfo, nullptr, &vertModule ) != VK_SUCCESS )
				assert( !"Failed to create shader module!" );

			VkPipelineShaderStageCreateInfo ssci[2] = {};

			ssci[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			ssci[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
			ssci[0].module = vertModule;
			ssci[0].pName = "main";
			ssci[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			ssci[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			ssci[1].module = fragModule;
			ssci[1].pName = "main";

			VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			VkPipelineInputAssemblyStateCreateInfo pipelineIACreateInfo = {};
			pipelineIACreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			pipelineIACreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

			VkViewport vp = {};
			vp.x = 0.0f;
			vp.y = 0.0f;
			vp.width = window.GetSize().m_Width;
			vp.height = window.GetSize().m_Height;
			vp.minDepth = 0.0f;
			vp.maxDepth = 1.0f;

			VkRect2D scissor = { 0 };
			scissor.extent.width = (uint32)window.GetSize().m_Width;
			scissor.extent.height = (uint32)window.GetSize().m_Height;
			scissor.offset.x = 0;
			scissor.offset.y = 0;

			VkPipelineViewportStateCreateInfo vpCreateInfo = {};
			vpCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			vpCreateInfo.viewportCount = 1;
			vpCreateInfo.scissorCount = 1;
			vpCreateInfo.pScissors = &scissor;
			vpCreateInfo.pViewports = &vp;

			VkPipelineDepthStencilStateCreateInfo dsInfo = {};
			dsInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			dsInfo.depthTestEnable = VK_TRUE;
			dsInfo.depthWriteEnable = VK_TRUE;
			dsInfo.depthCompareOp = VK_COMPARE_OP_LESS;
			dsInfo.depthBoundsTestEnable = VK_FALSE;
			dsInfo.minDepthBounds = 0.f;
			dsInfo.maxDepthBounds = 1.f;
			dsInfo.stencilTestEnable = VK_FALSE;
			dsInfo.front = {};
			dsInfo.back = {};

			VkPipelineRasterizationStateCreateInfo rastCreateInfo = {};
			rastCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rastCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
			rastCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
			rastCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rastCreateInfo.lineWidth = 1.0f;

			VkPipelineMultisampleStateCreateInfo pipelineMSCreateInfo = {};
			pipelineMSCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			pipelineMSCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

			VkPipelineColorBlendAttachmentState blendAttachState = {};
			blendAttachState.colorWriteMask = 0xf;

			VkPipelineColorBlendStateCreateInfo blendCreateInfo = {};
			blendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			blendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
			blendCreateInfo.attachmentCount = 1;
			blendCreateInfo.pAttachments = &blendAttachState;

			VkGraphicsPipelineCreateInfo pipelineInfo = {};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = ARRSIZE( ssci );
			pipelineInfo.pStages = &ssci[0];
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &pipelineIACreateInfo;
			pipelineInfo.pViewportState = &vpCreateInfo;
			pipelineInfo.pDepthStencilState = &dsInfo;
			pipelineInfo.pRasterizationState = &rastCreateInfo;
			pipelineInfo.pMultisampleState = &pipelineMSCreateInfo;
			pipelineInfo.pColorBlendState = &blendCreateInfo;
			pipelineInfo.renderPass = _renderPass;
			pipelineInfo.basePipelineIndex = -1;

			if( vkCreateGraphicsPipelines( m_LogicalDevice->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &_pipeline ) != VK_SUCCESS )
				assert( !"Failed to create pipeline!" );
		}

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

		VkImageSubresourceRange imageRange = {};
		imageRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageRange.levelCount = 1;
		imageRange.layerCount = 1;

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _renderPass;
		renderPassInfo.renderArea.offset.x = 0;
		renderPassInfo.renderArea.offset.y = 0;
		renderPassInfo.renderArea.extent.width = (uint32)_size.m_Width;
		renderPassInfo.renderArea.extent.height = (uint32)_size.m_Height;
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearValue;

		VkViewport viewport = { 0 };
		viewport.height = _size.m_Height;
		viewport.width = _size.m_Width;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = { 0 };
		scissor.extent.width = (uint32)_size.m_Width;
		scissor.extent.height = (uint32)_size.m_Height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;

		for( size_t i = 0; i < m_CmdBuffers.size(); ++i )
		{
			if( vkBeginCommandBuffer( m_CmdBuffers[i], &cmdInfo ) != VK_SUCCESS )
				assert( !"Failed to begin commandbuffer!" );

			std::vector<VkImage>& images = m_Swapchain->GetImageList();
			vkCmdClearColorImage( m_CmdBuffers[i], images.front(), VK_IMAGE_LAYOUT_GENERAL, &_clearColor, 1, &imageRange );

			renderPassInfo.framebuffer = m_FrameBuffers[i];
			vkCmdBeginRenderPass( m_CmdBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
			vkCmdBindPipeline( m_CmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline );

			vkCmdSetViewport( m_CmdBuffers[i], 0, 1, &viewport );

			vkCmdSetScissor( m_CmdBuffers[i], 0, 1, &scissor );
			vkCmdDraw( m_CmdBuffers[i], 3, 1, 0, 0 );

			vkCmdEndRenderPass( m_CmdBuffers[i] );

			vkEndCommandBuffer( m_CmdBuffers[i] );
		}

		if( vkAcquireNextImageKHR( m_LogicalDevice->GetDevice(), m_Swapchain->GetSwapchain(), UINT64_MAX, NULL, NULL, &m_Index ) != VK_SUCCESS )
			assert( !"Failed to acquire next image!" );

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CmdBuffers[m_Index];

		if( vkQueueSubmit( m_LogicalDevice->GetQueue(), 1, &submitInfo, nullptr ) != VK_SUCCESS )
			assert( !"Failed to submit the queue!" );

		VkSwapchainKHR swapchain = m_Swapchain->GetSwapchain();

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &m_Index;

		if( vkQueuePresentKHR( m_LogicalDevice->GetQueue(), &presentInfo ) != VK_SUCCESS )
			assert( !"Failed to present!" );
	}

}; //namespace Graphics