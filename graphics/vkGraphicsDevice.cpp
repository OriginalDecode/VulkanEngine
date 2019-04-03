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

VkClearColorValue _clearColor = { 0.f, 0.f, 0.f, 0.f };

VkRenderPass _renderPass = nullptr;

std::vector<VkFramebuffer> m_FrameBuffers;

namespace Graphics
{
	vkGraphicsDevice::vkGraphicsDevice() = default;

	vkGraphicsDevice::~vkGraphicsDevice() = default;

	bool vkGraphicsDevice::Init( const Window& window )
	{

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
			fbInfo.width = window.GetSize().m_Width;
			fbInfo.height = window.GetSize().m_Height;
			fbInfo.layers = 1;
			
			if( vkCreateFramebuffer( m_LogicalDevice->GetDevice(), &fbInfo, nullptr, &m_FrameBuffers[i] ) != VK_SUCCESS )
				assert( !"Failed to create framebuffer!" );


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

		for( size_t i = 0; i < m_CmdBuffers.size(); ++i )
		{
			if( vkBeginCommandBuffer( m_CmdBuffers[i], &cmdInfo ) != VK_SUCCESS )
				assert( !"Failed to begin commandbuffer!" );

			std::vector<VkImage>& images = m_Swapchain->GetImageList();

			vkCmdClearColorImage( m_CmdBuffers[i], images.front(), VK_IMAGE_LAYOUT_GENERAL, &_clearColor, 1, &imageRange );

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

		/*VkRenderPass renderPass = nullptr;
		VkPipeline pipeline = nullptr;
		VkCommandBuffer cmd = nullptr;

		VkPipelineLayout layout = nullptr;

		VkCommandBufferBeginInfo cmdInfo = {};
		VkRenderPassBeginInfo passInfo = {};

		vkBeginCommandBuffer( cmd, &cmdInfo );

		vkCmdBeginRenderPass( cmd, &passInfo, VK_SUBPASS_CONTENTS_INLINE );

		vkCmdBindPipeline( cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );

		vkCmdBindDescriptorSets( cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 1, 1, 0, 0, nullptr );

		VkViewport viewport;

		vkCmdSetViewport( cmd, 0, 1, &viewport );

		vkCmdDraw( cmd, 3, 1, 0, 0 );
		vkCmdEndRenderPass( cmd );
		vkEndCommandBuffer( cmd );

		VkSubmitInfo submitInfo = {};
		vkQueueSubmit( nullptr, 1, &submitInfo, nullptr );

		VkPresentInfoKHR presentInfo = {};
		vkQueuePresentKHR( nullptr, &presentInfo );*/
	}

}; //namespace Graphics