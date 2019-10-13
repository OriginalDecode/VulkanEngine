#include "VulkanRenderer.h"

#include "Core/File.h"
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

#include "graphics/vulkan/VulkanDevice.h"

namespace Graphics
{
	VulkanRenderer::~VulkanRenderer()
	{

		m_Device->DestroyObject(&vkDestroyPipelineLayout, m_PipelineLayout, nullptr);
		m_Device->DestroyObject(&vkDestroyPipeline, m_Pipeline, nullptr);

		m_Device->DestroyObject(&vkDestroySemaphore, m_DrawDone, nullptr);
		m_Device->DestroyObject(&vkDestroySemaphore, m_NextImage, nullptr);

		m_Device->FreeCommandBuffers(m_CommandPool, m_CommandsBuffers, m_NofCommandBuffers);

		m_Device->DestroyObject(&vkDestroyCommandPool, m_CommandPool, nullptr);

		m_Device->DestroyObject(&vkDestroyFence, m_CommandFence, nullptr);

		for(int32 i = 0; i < m_NofFrameBuffers; ++i)
			m_Device->DestroyObject(&vkDestroyFramebuffer, m_FrameBuffers[i], nullptr);

		delete[] m_CommandsBuffers;
		m_CommandsBuffers = nullptr;

		delete m_Device;
		m_Device = nullptr;
	}

	bool VulkanRenderer::Init(const Window& window)
	{
		const Window::Size& windowSize = window.GetSize();
		m_Device = new VulkanDevice;
		m_Device->Init();

		m_CommandPool = m_Device->CreateCommandPool();

		m_CommandsBuffers = new VkCommandBuffer[m_Device->GetSwapchainImageCount()];
		m_Device->CreateCommandBuffers(m_CommandPool, m_CommandsBuffers,
									   m_Device->GetSwapchainImageCount());

		SetupRenderPass();

		m_NofFrameBuffers = m_Device->GetSwapchainImageCount();
		m_FrameBuffers = new VkFramebuffer[m_NofFrameBuffers];

		VkImage* images = m_Device->GetImageList();
		VkImageView* viewList = m_Device->GetViewList();

		for(int i = 0; i < m_NofFrameBuffers; i++)
		{
			viewList[i] =
				m_Device->Create2DImageView(m_Device->GetPhysicalDeviceSurfaceFormat().format,
											images[i], VK_IMAGE_ASPECT_COLOR_BIT);

			VkImageView views[] = { viewList[i], m_Device->GetDepthView() };
			m_FrameBuffers[i] = m_Device->CreateFramebuffer(
				views, ARRSIZE(views), windowSize.m_Width, windowSize.m_Height, m_RenderPass);
		}

		m_Viewport.height = windowSize.m_Height;
		m_Viewport.width = windowSize.m_Width;
		m_Viewport.x = 0.f;
		m_Viewport.y = 0.f;
		m_Viewport.minDepth = 0.f;
		m_Viewport.maxDepth = 1.f;

		m_ScissorArea.extent = { (uint32)windowSize.m_Width, (uint32)windowSize.m_Height };
		m_ScissorArea.offset = { 0, 0 };

		m_Device->CreateDeviceSemaphore(&m_NextImage);
		m_Device->CreateDeviceSemaphore(&m_DrawDone);
		m_Device->CreateFence(&m_CommandFence);

		return true;

		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.descriptorCount = 1;

		VkDescriptorSetLayoutBinding bindings[] = {
			uboLayoutBinding,
		};

		SetupDescriptorLayout(bindings, ARRSIZE(bindings));

		VkDescriptorSetLayout descriptorLayouts[] = {
			m_DescriptorLayout,
		};

		VkPushConstantRange pushRangeList[] = {
			{ VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Core::Matrix44f) },
		};

		m_PipelineLayout = CreatePipelineLayout(descriptorLayouts, ARRSIZE(descriptorLayouts),
												pushRangeList, ARRSIZE(pushRangeList));
		m_Pipeline = CreateGraphicsPipeline();

		m_Device->CreateImGuiContext(m_RenderPass, m_DescriptorPool, m_CommandPool);
		return true;
	}
	//_____________________________________________

	void VulkanRenderer::DrawFrame(float dt)
	{

		m_Device->AcquireNextImage(m_DrawDone, nullptr, &m_Index);

		// SetupRenderCommands(m_Index ^ 1);

		const VkPipelineStageFlags waitDstStageMask =
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // associated with
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

		m_Device->SubmitQueue(submitInfo, 1, m_CommandFence);
		m_Device->PresentQueue(m_Index, &m_DrawDone, 1);

		m_Device->WaitForFences(&m_CommandFence, 1, VK_TRUE);
		m_Device->ResetFences(&m_CommandFence, 1);
	}

	void VulkanRenderer::SetupRenderCommands(int index)
	{
		const Window::Size& windowSize = GraphicsEngine::Get().GetWindow()->GetSize();

		VkClearValue clearValue[2] = {};
		clearValue[0].color = { 0.f, 0.f, 0.f, 0.f };
		clearValue[1].depthStencil = { 1.f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = { (uint32)windowSize.m_Width,
											 (uint32)windowSize.m_Height };
		renderPassInfo.clearValueCount = ARRSIZE(clearValue);
		renderPassInfo.pClearValues = clearValue;

		VkCommandBufferBeginInfo cmdInfo = {};
		cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		VkFramebuffer& frameBuffer = m_FrameBuffers[index];
		VkCommandBuffer& commandBuffer = m_CommandsBuffers[index];

		if(vkBeginCommandBuffer(commandBuffer, &cmdInfo) != VK_SUCCESS)
			ASSERT(false, "Failed to begin CommandBuffer!");
		renderPassInfo.framebuffer = frameBuffer;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0,
								1, &m_DescriptorSet, 0, nullptr);

		vkCmdEndRenderPass(commandBuffer);

		if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			ASSERT(false, "Failed to end CommandBuffer!");
	}

	//_____________________________________________

	void VulkanRenderer::SetupRenderPass()
	{

		VkAttachmentDescription attDesc = {};
		attDesc.format = m_Device->GetPhysicalDeviceSurfaceFormat().format;
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
		depthAttachment.format = m_Device->GetDepthFormat();
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
		dependency.dstAccessMask =
			VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkAttachmentDescription attachments[] = { attDesc, depthAttachment };
		VkRenderPassCreateInfo rpInfo = {};
		rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		rpInfo.attachmentCount = ARRSIZE(attachments);
		rpInfo.pAttachments = attachments;
		rpInfo.subpassCount = 1;
		rpInfo.pSubpasses = &subpassDesc;
		rpInfo.dependencyCount = 1;
		rpInfo.pDependencies = &dependency;

		m_Device->CreateObject(&vkCreateRenderPass, &rpInfo, &m_RenderPass);
	}
	//_____________________________________________

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
		VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
		pipelineDepthStencilStateCreateInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
		pipelineDepthStencilStateCreateInfo.minDepthBounds = 0.f;
		pipelineDepthStencilStateCreateInfo.maxDepthBounds = 1.f;
		pipelineDepthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
		pipelineDepthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
		pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
		pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
		pipelineDepthStencilStateCreateInfo.front = pipelineDepthStencilStateCreateInfo.back;

		// rasterizer
		VkPipelineRasterizationStateCreateInfo rastCreateInfo = {};
		rastCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rastCreateInfo.polygonMode = VK_POLYGON_MODE_FILL; // VK_POLYGON_MODE_LINE
														   // Wireframe
		rastCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rastCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rastCreateInfo.depthClampEnable = VK_FALSE;
		rastCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rastCreateInfo.depthBiasEnable = VK_FALSE;
		rastCreateInfo.lineWidth = 1.0f;

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

		auto bindDesc = CreateBindDesc();
		auto attrDesc = CreateAttrDesc(0, 0);
		auto attrDesc2 = CreateAttrDesc(1, 16);

		VkVertexInputBindingDescription bindDescArr[] = { bindDesc };
		VkVertexInputAttributeDescription descriptions[] = { attrDesc, attrDesc2 };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		vertexInputInfo.vertexBindingDescriptionCount = ARRSIZE(bindDescArr);
		vertexInputInfo.vertexAttributeDescriptionCount = ARRSIZE(descriptions);

		vertexInputInfo.pVertexBindingDescriptions = bindDescArr;
		vertexInputInfo.pVertexAttributeDescriptions = descriptions;

		VkDescriptorPoolSize poolSize = {};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = 1;

		m_DescriptorPool =
			m_Device->CreateDescriptorPool(&poolSize, 1, m_Device->GetSwapchainImageCount());

		VkDescriptorSetLayout layouts[] = { m_DescriptorLayout };
		m_DescriptorSet = m_Device->AllocDescriptorSet(m_DescriptorPool, 1, layouts);

		// VkDescriptorBufferInfo bInfo2 = {};

		// bInfo2.buffer = static_cast<VkBuffer>( _ViewProjection.GetBuffer() );
		// bInfo2.offset = 0;
		// bInfo2.range = _ViewProjection.GetSize();

		/*VkWriteDescriptorSet descWrite = {};
		descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descWrite.dstSet = m_DescriptorSet;
		descWrite.dstBinding = 0;
		descWrite.dstArrayElement = 0;
		descWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descWrite.descriptorCount = 1;
		descWrite.pBufferInfo = &bInfo2;

		m_Device->UpdateDescriptorSets(1, &descWrite, 0, nullptr);*/

		VkPipelineInputAssemblyStateCreateInfo pipelineIACreateInfo = {};
		pipelineIACreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipelineIACreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipelineIACreateInfo.primitiveRestartEnable = VK_FALSE;

		// VkPipelineShaderStageCreateInfo ssci[] = {
		//	CreateShaderStageInfo( VK_SHADER_STAGE_VERTEX_BIT,
		//_vertexShader.GetModule(), "main" ), 	CreateShaderStageInfo(
		// VK_SHADER_STAGE_FRAGMENT_BIT, _fragmentShader.GetModule(), "main" )
		//};

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

		// pipelineInfo.pStages = ssci;
		// pipelineInfo.stageCount = ARRSIZE( ssci );

		VkPipeline pipeline;
		m_Device->CreatePipelines(nullptr, 1, &pipelineInfo, &pipeline);

		// DestroyShader( &_vertexShader );
		// DestroyShader( &_fragmentShader );

		return pipeline;
	}

	void VulkanRenderer::SetupDescriptorLayout(VkDescriptorSetLayoutBinding* descriptorBindings,
											   int32 bindingCount)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = bindingCount;
		layoutInfo.pBindings = descriptorBindings;

		m_Device->CreateObject(&vkCreateDescriptorSetLayout, &layoutInfo, &m_DescriptorLayout);
	}

	//_____________________________________________

	VkPipelineLayout VulkanRenderer::CreatePipelineLayout(VkDescriptorSetLayout* descriptorLayouts,
														  int32 descriptorLayoutCount,
														  VkPushConstantRange* pushConstantRange,
														  int32 pushConstantRangeCount)
	{

		VkPipelineLayoutCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		pipelineCreateInfo.setLayoutCount = descriptorLayoutCount;
		pipelineCreateInfo.pSetLayouts = descriptorLayouts;

		pipelineCreateInfo.pushConstantRangeCount = pushConstantRangeCount;
		pipelineCreateInfo.pPushConstantRanges = pushConstantRange;

		VkPipelineLayout pipeline;
		m_Device->CreateObject(&vkCreatePipelineLayout, &pipelineCreateInfo, &pipeline);

		return pipeline;
	}
	//_____________________________________________

	VkVertexInputBindingDescription VulkanRenderer::CreateBindDesc()
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		// bindingDescription.stride = sizeof( Vertex );
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	VkVertexInputAttributeDescription VulkanRenderer::CreateAttrDesc(int location, int offset)
	{
		VkVertexInputAttributeDescription attrDesc = {};
		attrDesc.binding = 0;
		attrDesc.location = location;
		attrDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attrDesc.offset = offset;
		return attrDesc;
	}

	//_____________________________________________

	//_____________________________________________

	VkShaderModule VulkanRenderer::LoadShader(const char* filepath, VkDevice pDevice)
	{
		Core::File shader(filepath, Core::FileMode::READ_FILE);
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shader.GetSize();
		createInfo.pCode = (const uint32_t*)shader.GetBuffer();

		VkShaderModule shaderModule = nullptr;
		if(vkCreateShaderModule(pDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			ASSERT(false, "Failed to create VkShaderModule!");

		return shaderModule;
	}

	void VulkanRenderer::LoadShader(HShader* shader, const char* filepath)
	{
		// shader->Create( LoadShader( filepath, m_LogicalDevice->GetDevice() )
		// );
	}

	void VulkanRenderer::DestroyShader(HShader* pShader)
	{
		// vkDestroyShaderModule( m_LogicalDevice->GetDevice(),
		// pShader->GetModule(), nullptr );
	}

	void VulkanRenderer::CreateConstantBuffer(ConstantBuffer* constantBuffer)
	{ /*
		 VkBufferCreateInfo createInfo = {};
		 createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		 createInfo.size = constantBuffer->GetSize();
		 createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		 createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		 auto lDevice = m_LogicalDevice->GetDevice();
		 auto pDevice = m_PhysicalDevice->GetDevice();

		 VkBuffer vulkan_buffer = nullptr;
		 if(vkCreateBuffer(lDevice, &createInfo, nullptr, &vulkan_buffer) != VK_SUCCESS)
			 ASSERT(false, "Failed to create vertex buffer!");

		 VkMemoryRequirements memRequirements;
		 vkGetBufferMemoryRequirements(lDevice, vulkan_buffer, &memRequirements);

		 VkDeviceMemory deviceMem = GPUAllocateMemory(memRequirements, lDevice, pDevice);

		 if(vkBindBufferMemory(lDevice, vulkan_buffer, deviceMem, 0) != VK_SUCCESS)
			 ASSERT(false, "Failed to bind buffer memory!");

		 constantBuffer->SetBuffer(vulkan_buffer);
		 constantBuffer->SetDeviceMemory(deviceMem);*/
	}

	void VulkanRenderer::DestroyConstantBuffer(ConstantBuffer* constantBuffer)
	{
		/*vkDestroyBuffer(m_LogicalDevice->GetDevice(),
						static_cast<VkBuffer>(constantBuffer->GetBuffer()), nullptr);*/
	}

	void VulkanRenderer::BindConstantBuffer(ConstantBuffer* constantBuffer, uint32 offset)
	{
		/*auto lDevice = m_LogicalDevice->GetDevice();

		VkDeviceMemory deviceMem = static_cast<VkDeviceMemory>(constantBuffer->GetDeviceMemory());
		void* data = nullptr;
		if(vkMapMemory(lDevice, deviceMem, offset, constantBuffer->GetSize(), 0, &data) !=
		   VK_SUCCESS)
			ASSERT(false, "Failed to map memory!");

		int8* pMem = static_cast<int8*>(data);
		int32 step = 0;
		for(auto it : constantBuffer->GetVariables())
		{
			memcpy(&pMem[step], static_cast<int8*>(it.var), it.size);
			step += it.size;
		}
		vkUnmapMemory(lDevice, deviceMem);*/
	}
}; // namespace Graphics
