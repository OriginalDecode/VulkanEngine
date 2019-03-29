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

VkClearValue _clearColor = { 0.f, 0.f, 0.f, 0.f };



namespace Graphics
{
	vkGraphicsDevice::vkGraphicsDevice() = default;


	vkGraphicsDevice::~vkGraphicsDevice() = default;

	bool vkGraphicsDevice::Init(const Window& window)
	{

		m_Instance = std::make_unique<VlkInstance>();
		m_Instance->Init();

		m_PhysicalDevice = std::make_unique<VlkPhysicalDevice>();
		m_PhysicalDevice->Init(m_Instance.get());

		m_LogicalDevice = std::make_unique<VlkDevice>();
		m_LogicalDevice->Init(m_PhysicalDevice.get());

		m_Swapchain = std::make_unique<VlkSwapchain>();
		m_Swapchain->Init(m_Instance.get(), m_LogicalDevice.get(), m_PhysicalDevice.get(), window);


		return true;
	}

	void vkGraphicsDevice::Present()
	{
	}

	void vkGraphicsDevice::DrawFrame()
	{
		VkPipelineLayout layout;
		VkPipeline pipeline = nullptr;
		VkCommandBufferBeginInfo cmdInfo = {};
		VkRenderPassBeginInfo passInfo = {};
		VkCommandBuffer cmd;
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
		vkQueuePresentKHR( nullptr, &presentInfo );

	}
	
}; //namespace Graphics