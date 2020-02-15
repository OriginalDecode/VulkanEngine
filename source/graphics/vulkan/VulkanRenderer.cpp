#include "VulkanRenderer.h"

#include "Core/math/Matrix44.h"
#include "Core/utilities/Randomizer.h"

#include "logger/Debug.h"
#include "graphics/GraphicsEngine.h"
#include "graphics/Utilities.h"
#include "graphics/Window.h"

#include <vulkan/vulkan.h>
#ifdef _WIN32
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif
#include "thirdparty/imgui/imgui.h"
#include "graphics/imgui/imgui_impl_vulkan.h"

#include "graphics/vulkan/VulkanUtils.h"
#include "graphics/RenderContextVulkan.h"
#include "graphics/vulkan/VulkanRasterizerStateInfos.h"
#include "graphics/vulkan/VulkanDepthStencilInfos.h"
#include "graphics/vulkanpipeline.h"

namespace Graphics
{
	VulkanRenderer::~VulkanRenderer()
	{

		VkDevice device = m_Context->Device;

		vkDestroyImage(device, m_DefaultDepthImage, nullptr);
		vkDestroyImageView(device, m_DefaultDepthImageView, nullptr);
		vkFreeMemory(device, m_DepthMemory, nullptr);
		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
		vkDestroyPipeline(device, m_Pipeline, nullptr);
		vkDestroySemaphore(device, m_DrawDone, nullptr);
		vkDestroySemaphore(device, m_NextImage, nullptr);
		vkDestroyCommandPool(device, m_CommandPool, nullptr);
		vkDestroyFence(device, m_CommandFence, nullptr);

		vkFreeCommandBuffers(device, m_CommandPool, m_NofCommandBuffers, m_CommandsBuffers);
		for(uint32 i = 0; i < m_NofFrameBuffers; i++)
			vkDestroyFramebuffer(device, m_FrameBuffers[i], nullptr);

		delete[] m_CommandsBuffers;
		m_CommandsBuffers = nullptr;
	}

	bool VulkanRenderer::Init(RenderContextVulkan* ctx)
	{
		m_Context = ctx;
		const Window::Size& window_size = ctx->Window->GetInnerSize();

		m_CommandPool = vlk::CreateCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, ctx->QueueFamily);

		m_CommandsBuffers = vlk::CreateCommandBuffers(m_CommandPool, ctx->SwapchainCtx.ImageCount);
		m_NofFrameBuffers = ctx->SwapchainCtx.ImageCount;
		m_FrameBuffers = new VkFramebuffer[m_NofFrameBuffers];

		SetupDepthResources();
		SetupRenderPass();
		auto [format_list, nof_formats] = vlk::AllocPhysicalDeviceSurfaceFormatsList(m_Context->Surface);

		for(uint32 i = 0; i < m_NofFrameBuffers; i++)
		{

			ctx->SwapchainCtx.Views[i] = vlk::Create2DImageView(format_list[0].format, ctx->SwapchainCtx.Images[i], VK_IMAGE_ASPECT_COLOR_BIT);

			VkImageView views[] = { ctx->SwapchainCtx.Views[i], m_DefaultDepthImageView };
			m_FrameBuffers[i] = vlk::CreateFramebuffer(views, ARRSIZE(views), (uint32)window_size.m_Width, (uint32)window_size.m_Height, m_RenderPass);
		}

		m_Viewport.height = window_size.m_Height;
		m_Viewport.width = window_size.m_Width;
		m_Viewport.x = 0.f;
		m_Viewport.y = 0.f;
		m_Viewport.minDepth = 0.f;
		m_Viewport.maxDepth = 1.f;

		m_ScissorArea.extent = { (uint32)window_size.m_Width, (uint32)window_size.m_Height };
		m_ScissorArea.offset = { 0, 0 };

		m_CommandFence = vlk::CreateFence();
		m_NextImage = vlk::CreateDeviceSemaphore();
		m_DrawDone = vlk::CreateDeviceSemaphore();

		VkDescriptorSetLayoutBinding uboLayoutBinding = vlk::CreateLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);

		VkDescriptorSetLayoutBinding bindings[] = {
			uboLayoutBinding,
		};

		VulkanPipeline pipeline();

		m_DescriptorLayout = vlk::CreateDescLayout(bindings, ARRSIZE(bindings));

		VkDescriptorSetLayout descriptorLayouts[] = {
			m_DescriptorLayout,
		};

		VkPushConstantRange pushRangeList[] = {
			{ VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Core::Matrix44f) },
		};

		m_PipelineLayout = vlk::CreatePipelineLayout(descriptorLayouts, ARRSIZE(descriptorLayouts), pushRangeList, ARRSIZE(pushRangeList));
		m_Pipeline = CreateGraphicsPipeline();

		// CreateImGuiContext();
		return true;
	}

	void VulkanRenderer::DrawFrame(float /*dt*/)
	{

		vkAcquireNextImageKHR(m_Context->Device, m_Context->SwapchainCtx.Swapchain, UINT64_MAX, m_NextImage, nullptr /* fence */, &m_Index);

		const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // associated with
																									 // having
																									 // semaphores
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitDstStageMask = &waitDstStageMask;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandsBuffers[m_Index];

		submitInfo.pSignalSemaphores = &m_DrawDone;
		submitInfo.signalSemaphoreCount = 1;

		submitInfo.pWaitSemaphores = &m_NextImage;
		submitInfo.waitSemaphoreCount = 1;

		VkResult result = vkQueueSubmit(m_Context->Queue, 1, &submitInfo, m_CommandFence);
		ASSERT(result == VK_SUCCESS, "Failed to submit queue. Reason : %s", vlk::GetFailReason(result));
		vlk::PresentQueue(m_Context->Queue, m_Index, &m_DrawDone, 1, m_Context->SwapchainCtx.Swapchain);

		SetupRenderCommands(m_Index ^ 1);
	}

	void VulkanRenderer::SetupRenderCommands(int index)
	{
		vkWaitForFences(m_Context->Device, 1, &m_CommandFence, VK_TRUE, UINT64_MAX);
		vkResetFences(m_Context->Device, 1, &m_CommandFence);

		const Window::Size& window_size = m_Context->Window->GetInnerSize();

		VkClearValue clear_value[2] = {};
		clear_value[0].color = { 0.f, 0.f, 0.f, 0.f };
		clear_value[1].depthStencil = { 1.f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = { (uint32)window_size.m_Width, (uint32)window_size.m_Height };
		renderPassInfo.clearValueCount = ARRSIZE(clear_value);
		renderPassInfo.pClearValues = clear_value;

		VkCommandBufferBeginInfo cmdInfo = {};
		cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		VkFramebuffer& frameBuffer = m_FrameBuffers[index];
		VkCommandBuffer& commandBuffer = m_CommandsBuffers[index];

		VkResult result = vkBeginCommandBuffer(commandBuffer, &cmdInfo);
		ASSERT(result == VK_SUCCESS, "Failed to begin CommandBuffer! Reason: %s", vlk::GetFailReason(result));

		renderPassInfo.framebuffer = frameBuffer;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);

		/* things here , like imgui or cubes or something else */

		vkCmdEndRenderPass(commandBuffer);

		result = vkEndCommandBuffer(commandBuffer);
		ASSERT(result == VK_SUCCESS, "Failed to end CommandBuffer!");
	}

	void VulkanRenderer::SetupRenderPass()
	{

		VkAttachmentDescription attDesc = {};

		auto [format_list, nof_formats] = vlk::AllocPhysicalDeviceSurfaceFormatsList(m_Context->Surface);

		VkSurfaceFormatKHR format = vlk::GetSurfaceFormat(format_list, nof_formats, VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

		vlk::FreePhysicalDeviceSurfaceFormatsList(format_list);

		attDesc.format = format.format;
		attDesc.samples = VK_SAMPLE_COUNT_1_BIT;

		attDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference attRef = {};
		attRef.attachment = 0;
		attRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = m_DepthFormat;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDesc = {};
		subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDesc.colorAttachmentCount = 1;
		subpassDesc.pColorAttachments = &attRef;
		subpassDesc.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkAttachmentDescription attachments[] = { attDesc, depthAttachment };
		VkRenderPassCreateInfo rpInfo = {};
		rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		rpInfo.attachmentCount = ARRSIZE(attachments);
		rpInfo.pAttachments = attachments;
		rpInfo.subpassCount = 1;
		rpInfo.pSubpasses = &subpassDesc;
		rpInfo.dependencyCount = 1;
		rpInfo.pDependencies = &dependency;

		vkCreateRenderPass(m_Context->Device, &rpInfo, nullptr, &m_RenderPass);
	}

	void VulkanRenderer::SetupDepthResources()
	{
		// get the depth format
		VkFormat depthFormat = {};
		VkFormat formats[] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };

		const VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
		for(VkFormat format : formats)
		{
			VkFormatProperties properties = {};
			vkGetPhysicalDeviceFormatProperties(m_Context->PhysicalDevice, format, &properties);

			if((properties.optimalTilingFeatures & features) == features)
				depthFormat = format;
		}

		const VkExtent2D extent = vlk::GetSurfaceCapabilities(m_Context->Surface).currentExtent;

		vlk::ImageCreateInfo create_info = {};
		create_info.Height = extent.height;
		create_info.Width = extent.width;
		create_info.ImageFormat = depthFormat;
		create_info.ImageTiling = VK_IMAGE_TILING_OPTIMAL;
		create_info.UsageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		create_info.MemoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		create_info.Offset = 0;

		auto [image, memory] = vlk::Create2DImage(create_info);

		m_DefaultDepthImage = image;
		m_DepthMemory = memory;
		m_DefaultDepthImageView = vlk::Create2DImageView(depthFormat, m_DefaultDepthImage, VK_IMAGE_ASPECT_DEPTH_BIT);
		m_DepthFormat = depthFormat;

		// setup a commandpool and commandbuffer
		VkCommandPool pool = vlk::CreateCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_Context->QueueFamily);
		const int32 nof_buffers = 1;
		VkCommandBuffer* buffer = vlk::CreateCommandBuffers(pool, nof_buffers);

		vlk::BeginSingleTimeCommand(buffer[0]);

		VkImageLayout oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageLayout newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_DefaultDepthImage;

		if(newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if(vlk::HasDepthStencilComponent(depthFormat))
			{
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage = {};
		VkPipelineStageFlags destinationStage = {};

		if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
		{
			ASSERT(false, "unsupported layout transition!");
		}

		vkCmdPipelineBarrier(buffer[0], sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		vlk::EndSingleTimeCommand(buffer[0], m_Context->Queue);

		vkFreeCommandBuffers(m_Context->Device, pool, nof_buffers, buffer);
		vkDestroyCommandPool(m_Context->Device, pool, nullptr);
		delete[] buffer;
	}

	VkPipeline VulkanRenderer::CreateGraphicsPipeline()
	{

		// viewport
		VkPipelineViewportStateCreateInfo vpCreateInfo = {};
		vpCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		vpCreateInfo.viewportCount = 1;
		vpCreateInfo.scissorCount = 1;
		vpCreateInfo.pScissors = &m_ScissorArea;
		vpCreateInfo.pViewports = &m_Viewport;

		// depth
		VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = vlk::depthStencilInfos[vlk::DEPTH_READ_WRITE];

		// rasterizer
		VkPipelineRasterizationStateCreateInfo rastCreateInfo = vlk::rasterizerInfos[vlk::FILL_CULL_BACK_CLOCKWISE];

		// sampler
		VkPipelineMultisampleStateCreateInfo pipelineMSCreateInfo = {};
		pipelineMSCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipelineMSCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pipelineMSCreateInfo.sampleShadingEnable = VK_FALSE;

		// blendstate
		VkPipelineColorBlendAttachmentState blendAttachState = {};
		blendAttachState.colorWriteMask = 0xF;
		blendAttachState.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo blendCreateInfo = {};
		blendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		blendCreateInfo.logicOpEnable = VK_FALSE;
		blendCreateInfo.attachmentCount = 1;
		blendCreateInfo.pAttachments = &blendAttachState;

		struct Vertex
		{
			Core::Vector4f pos;
			Core::Vector4f color;
		};

		/* */
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		/* input layout "technically" */
		auto bindDesc = vlk::CreateBindingDesc(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
		VkVertexInputBindingDescription bindDescArr[] = { bindDesc };
		vertexInputInfo.vertexBindingDescriptionCount = ARRSIZE(bindDescArr);

		/*
			Describes each input
		*/
		auto attrDesc = vlk::CreateAttributeDesc(VK_FORMAT_R32G32B32A32_SFLOAT, 0, 0, 0);						/* float4 position */
		auto attrDesc2 = vlk::CreateAttributeDesc(VK_FORMAT_R32G32B32A32_SFLOAT, 0, 1, sizeof(Core::Vector4f)); /* float4 color */

		VkVertexInputAttributeDescription descriptions[] = { attrDesc, attrDesc2 };
		vertexInputInfo.vertexAttributeDescriptionCount = ARRSIZE(descriptions);

		vertexInputInfo.pVertexBindingDescriptions = bindDescArr;
		vertexInputInfo.pVertexAttributeDescriptions = descriptions;

		VkDescriptorPoolSize poolSize = {};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = 1;

		m_DescriptorPool = vlk::CreateDescPool(&poolSize, 1, m_Context->SwapchainCtx.ImageCount);

		VkDescriptorSetLayout layouts[] = { m_DescriptorLayout };
		m_DescriptorSet = vlk::AllocDescSet(m_DescriptorPool, 1, layouts);

		/*

		VkDescriptorBufferInfo bInfo2 = {};

		bInfo2.buffer = static_cast<VkBuffer>( _ViewProjection.GetBuffer() );
		bInfo2.offset = 0;
		bInfo2.range = sizeof(Core::Matrix44f) * 2;

		VkWriteDescriptorSet descWrite = {};
		descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descWrite.dstSet = m_DescriptorSet;
		descWrite.dstBinding = 0;
		descWrite.dstArrayElement = 0;
		descWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descWrite.descriptorCount = 1;
		descWrite.pBufferInfo = &bInfo2;

		*/

		// m_Device->UpdateDescriptorSets(1, &descWrite, 0, nullptr);

		VkPipelineInputAssemblyStateCreateInfo pipelineIACreateInfo = {};
		pipelineIACreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipelineIACreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipelineIACreateInfo.primitiveRestartEnable = VK_FALSE;

		VkShaderModule vert = vlk::LoadShader("Data/shaders/vertex.vert");
		VkShaderModule frag = vlk::LoadShader("Data/shaders/frag.frag");

		VkPipelineShaderStageCreateInfo ssci[] = { vlk::CreateShaderStageInfo(VK_SHADER_STAGE_VERTEX_BIT, vert, "main"),
												   vlk::CreateShaderStageInfo(VK_SHADER_STAGE_FRAGMENT_BIT, frag, "main") };

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &pipelineIACreateInfo;
		pipelineInfo.renderPass = m_RenderPass;
		pipelineInfo.pViewportState = &vpCreateInfo;
		pipelineInfo.pColorBlendState = &blendCreateInfo;
		pipelineInfo.pRasterizationState = &rastCreateInfo;
		pipelineInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
		pipelineInfo.pMultisampleState = &pipelineMSCreateInfo;

		pipelineInfo.pStages = ssci;
		pipelineInfo.stageCount = ARRSIZE(ssci);

		VkPipeline pipeline;
		VkResult result = vkCreateGraphicsPipelines(m_Context->Device, nullptr, 1, &pipelineInfo, nullptr, &pipeline);
		ASSERT(result == VK_SUCCESS, "Failed to create pipeline!");

		vlk::DestroyShader(vert);
		vlk::DestroyShader(frag);

		return pipeline;
	}

	void VulkanRenderer::CreateImGuiContext()
	{
		const Window::Size& window_size = m_Context->Window->GetSize();
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize.x = window_size.m_Width;
		io.DisplaySize.y = window_size.m_Height;

		ImGui_ImplVulkan_InitInfo info = {};
		info.Device = m_Context->Device;
		info.PhysicalDevice = m_Context->PhysicalDevice;
		info.Instance = m_Context->Instance;
		info.Queue = m_Context->Queue;
		info.QueueFamily = m_Context->QueueFamily;
		info.DescriptorPool = m_DescriptorPool;
		info.MinImageCount = 2;
		info.ImageCount = m_Context->SwapchainCtx.ImageCount;

		if(!ImGui_ImplVulkan_Init(&info, m_RenderPass))
			ASSERT(false, "Failed");

		VkCommandPool pool = vlk::CreateCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_Context->QueueFamily);

		VkResult result = vkResetCommandPool(m_Context->Device, pool, 0);
		ASSERT(result == VK_SUCCESS, "failed to reset commandPool");

		const int32 nof_buffers = 1;
		VkCommandBuffer* buffer = vlk::CreateCommandBuffers(pool, nof_buffers);

		vlk::BeginSingleTimeCommand(buffer[0]);

		ImGui_ImplVulkan_CreateFontsTexture(buffer[0]);

		vlk::EndSingleTimeCommand(buffer[0], m_Context->Queue);

		vkFreeCommandBuffers(m_Context->Device, pool, nof_buffers, buffer);
		delete[] buffer;
		vkDestroyCommandPool(m_Context->Device, pool, nullptr);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

}; // namespace Graphics
