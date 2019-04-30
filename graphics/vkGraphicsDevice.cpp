#include "vkGraphicsDevice.h"

#include "Core/File.h"
#include "Core/math/Matrix44.h"

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
VkShaderModule vertModule = nullptr;
VkShaderModule fragModule = nullptr;
VkPipeline _pipeline = nullptr;
VkPipelineLayout _pipelineLayout = nullptr;
VkViewport _Viewport = {};
VkRect2D _Scissor = {};
VkDescriptorSetLayout _descriptorLayout = nullptr;
VkDescriptorPool _descriptorPool = nullptr;
VkDescriptorSet _descriptorSet = nullptr;


Core::Matrix44f _worldMatrix;
Core::Matrix44f _worldMatrix2;
Core::Matrix44f _projectionMatrix;
Core::Matrix44f _translationMatrix;
Core::Matrix44f _ViewProjectionMatrix;

VkBuffer _VertexBuffer;
VkDeviceMemory _vertexBufferMemory;

VkBuffer _cubeBuffer;
VkDeviceMemory _cubeBufferMemory;


VkBuffer _MatrixBuffer;
VkDeviceMemory _matrixBufferMemory;

Window::Size _size;
Core::Vector4f _triangle[3] = {
	{ 0.0f, -0.75f, 0.f, 1.f },
	{ 0.25f, 0.5f, 0.f, 1.f },
	{ -0.25f, 0.5f, 0.f, 1.f }
};

struct Vertex
{
	Core::Vector4f position;
	Core::Vector4f color;
};

constexpr Vertex _cube[] = {
	//front
	{ { -1.f, -1.f, -1.f }, { 1.f,0.f,0.f } },
	{ {  1.f, -1.f, -1.f }, { 1.f,0.f,0.f } },
	{ {  1.f,  1.f, -1.f }, { 1.f,0.f,0.f } },

	{ { -1.f, -1.f, -1.f }, { 1.f,0.f,0.f } },
	{ {  1.f,  1.f, -1.f }, { 1.f,0.f,0.f } },
	{ { -1.f,  1.f, -1.f }, { 1.f,0.f,0.f } },

	//right
	{ { 1.f, -1.f, -1.f }, { 0.f, 1.f, 0.f } },
	{ { 1.f, -1.f,  1.f }, { 0.f, 1.f, 0.f } },
	{ { 1.f,  1.f,  1.f }, { 0.f, 1.f, 0.f } },

	{ { 1.f, -1.f, -1.f }, { 0.f, 1.f, 0.f } },
	{ { 1.f, 1.f,   1.f }, { 0.f, 1.f, 0.f } },
	{ { 1.f, 1.f,  -1.f }, { 0.f, 1.f, 0.f } },

	//back
	{ {	 1.f, -1.f, 1.f }, { 0.f, 0.f, 1.f } },
	{ { -1.f, -1.f, 1.f }, { 0.f, 0.f, 1.f } },
	{ { -1.f,  1.f, 1.f }, { 0.f, 0.f, 1.f } },

	{ {  1.f, -1.f, 1.f }, { 0.f, 0.f, 1.f } },
	{ { -1.f,  1.f, 1.f }, { 0.f, 0.f, 1.f } },
	{ {  1.f,  1.f, 1.f }, { 0.f, 0.f, 1.f } },

	//left
	{ { -1.f, -1.f,  1.f }, { 1.f, 1.f, 0.f } },
	{ { -1.f, -1.f, -1.f }, { 1.f, 1.f, 0.f } },
	{ { -1.f,  1.f, -1.f }, { 1.f, 1.f, 0.f } },

	{ { -1.f, -1.f,  1.f }, { 1.f, 1.f, 0.f } },
	{ { -1.f,  1.f, -1.f }, { 1.f, 1.f, 0.f } },
	{ { -1.f,  1.f, 1.f }, { 1.f, 1.f, 0.f } },

	//top
	{ { -1.f, 1.f, -1.f }, { 1.f, 1.f, 1.f } },
	{ {  1.f, 1.f, -1.f }, { 1.f, 1.f, 1.f } },
	{ {  1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f } },

	{ { 1.f, 1.f, 1.f },{ 1.f, 1.f, 1.f } },
	{ { -1.f, 1.f, 1.f }, { 1.f, 1.f, 1.f } },
	{ { -1.f, 1.f, -1.f },{ 1.f, 1.f, 1.f } },

	//bottom
	{ { 1.f, -1.f, -1.f },{ 0.f, 1.f, 1.f } },
	{ { -1.f, -1.f, -1.f },{ 0.f, 1.f, 1.f } },
	{ { 1.f, -1.f, 1.f },{ 0.f, 1.f, 1.f } },

	{ { -1.f, -1.f, 1.f },{ 0.f, 1.f, 1.f } },
	{ { 1.f, -1.f, 1.f },{ 0.f, 1.f, 1.f } },
	{ { -1.f, -1.f, -1.f },{ 0.f, 1.f, 1.f } },

};
#include <vector>
namespace Graphics
{
	uint32 findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags, VkPhysicalDevice device);
	VkDeviceMemory GPUAllocateMemory(const VkMemoryRequirements& memRequirements, VkDevice logicalDevice, VkPhysicalDevice physDevice)
	{
		VkDeviceMemory memory;
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, physDevice);

		if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &memory) != VK_SUCCESS)
			assert(!"Failed to allocate memory on GPU!");

		return memory;
	}

	void ConstantBuffer::Init(VkDevice logicDevice, VkPhysicalDevice physDevice)
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = m_BufferSize;
		createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(logicDevice, &createInfo, nullptr, &m_cBuffer) != VK_SUCCESS)
			assert(!"Failed to create vertex buffer!");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(logicDevice, m_cBuffer, &memRequirements);

		m_DeviceMemory = GPUAllocateMemory(memRequirements, logicDevice, physDevice);

		if (vkBindBufferMemory(logicDevice, m_cBuffer, m_DeviceMemory, 0) != VK_SUCCESS)
			assert(!"Failed to bind buffer memory!");
	}

	
	void ConstantBuffer::Bind(VkDevice pDevice, int offset)
	{
		void* data = nullptr;
		if (vkMapMemory(pDevice, m_DeviceMemory, offset, m_BufferSize, 0, &data) != VK_SUCCESS)
			assert(!"Failed to map memory!");
		memcpy(&static_cast<int8*>(data)[0], &m_Vars[0], m_BufferSize);
		vkUnmapMemory(pDevice, m_DeviceMemory);
	}
	ConstantBuffer _Model;
	ConstantBuffer _Model2;
	ConstantBuffer _ViewProjection;

	vkGraphicsDevice::vkGraphicsDevice() = default;

	vkGraphicsDevice::~vkGraphicsDevice()
	{

		auto device = m_LogicalDevice->GetDevice();
		_Model.Destroy(device);
		_Model2.Destroy(device);
		_ViewProjection.Destroy(device);

		vkDestroyBuffer(device, _cubeBuffer, nullptr);
		vkDestroyBuffer(device, _VertexBuffer, nullptr);
		vkDestroyBuffer(device, _MatrixBuffer, nullptr);
		vkDestroyShaderModule(device, vertModule, nullptr);
		vkDestroyShaderModule(device, fragModule, nullptr);
		vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);
		vkDestroyPipeline(device, _pipeline, nullptr);

		vkDestroySemaphore(device, m_DrawDone, nullptr);
		vkDestroySemaphore(device, m_AcquireNextImageSemaphore, nullptr);
		vkDestroyCommandPool(device, m_CmdPool, nullptr);

		for (VkFramebuffer buffer : m_FrameBuffers)
			vkDestroyFramebuffer(device, buffer, nullptr);
	}



	bool vkGraphicsDevice::Init(const Window& window)
	{
		_size = window.GetInnerSize();

		_projectionMatrix = Core::Matrix44f::CreateProjectionMatrixLH(0.01f, 100.f, _size.m_Height / _size.m_Width, 90.f * (3.1415f / 180.f));
		_translationMatrix = Core::Matrix44f::Identity();
		_translationMatrix = Core::Matrix44f::Inverse(_translationMatrix);
		_worldMatrix = Core::Matrix44f::Identity();
		_worldMatrix.SetPosition({ 0.f, 0.f, 5.0f });

		_worldMatrix2 = Core::Matrix44f::Identity();
		_worldMatrix2.SetPosition({ 0.f, 0.f, 2.f });

		m_Instance = std::make_unique<VlkInstance>();
		m_Instance->Init();

		m_PhysicalDevice = std::make_unique<VlkPhysicalDevice>();
		m_PhysicalDevice->Init(m_Instance.get());

		m_LogicalDevice = std::make_unique<VlkDevice>();
		m_LogicalDevice->Init(m_PhysicalDevice.get());

		m_Swapchain = std::make_unique<VlkSwapchain>();
		m_Swapchain->Init(m_Instance.get(), m_LogicalDevice.get(), m_PhysicalDevice.get(), window);

		CreateVertexBuffer();
		//CreateMatrixBuffer();


		auto pDevice = m_PhysicalDevice->GetDevice();
		auto lDevice = m_LogicalDevice->GetDevice();

		_ViewProjection.RegVar(&_worldMatrix);
		_ViewProjection.RegVar(&_ViewProjectionMatrix);
		_ViewProjection.Init(lDevice, pDevice);

		CreateCube();

		CreateCommandPool();
		CreateCommandBuffer();
		CreateRenderPass();


		CreateFramebuffers();

		vertModule = LoadShader("Data/Shaders/vertex.vert", m_LogicalDevice->GetDevice());
		fragModule = LoadShader("Data/Shaders/frag.frag", m_LogicalDevice->GetDevice());

		SetupViewport();
		SetupScissorArea();
		CreateDescriptorLayout();
		CreatePipelineLayout();
		CreateGraphicsPipeline();
		
		m_AcquireNextImageSemaphore = CreateVkSemaphore(m_LogicalDevice->GetDevice());
		m_DrawDone = CreateVkSemaphore(m_LogicalDevice->GetDevice());


		VkClearValue clearValue = {};
		clearValue.color = _clearColor;

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _renderPass;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = { (uint32)_size.m_Width, (uint32)_size.m_Height };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearValue;

		VkCommandBufferBeginInfo cmdInfo = {};
		cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		for (size_t i = 0; i < m_CmdBuffers.size(); ++i)
		{

			VkFramebuffer& frameBuffer = m_FrameBuffers[i];
			VkCommandBuffer& commandBuffer = m_CmdBuffers[i];


			if (vkBeginCommandBuffer(commandBuffer, &cmdInfo) != VK_SUCCESS)
				assert(!"Failed to begin CommandBuffer!");
			renderPassInfo.framebuffer = frameBuffer;

			vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_descriptorSet, 0, nullptr);

			//VkBuffer vertexBuffers[] = { _VertexBuffer };
			VkDeviceSize offsets = { 0 };
			//vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, &offsets);

			//_Model.Bind(m_LogicalDevice->GetDevice(), 0);
			//vkCmdDraw(commandBuffer, 3, 1, 0, 0);

			//_Model2.Bind(m_LogicalDevice->GetDevice(), 0);
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_cubeBuffer, &offsets);
			vkCmdDraw(commandBuffer, ARRSIZE(_cube), 1, 0, 0);


			vkCmdEndRenderPass(commandBuffer);

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
				assert(!"Failed to end CommandBuffer!");
		}

		return true;
	}
	//_____________________________________________

	void vkGraphicsDevice::SetupScissorArea()
	{
		_Scissor.extent.width = (uint32)_size.m_Width;
		_Scissor.extent.height = (uint32)_size.m_Height;
		_Scissor.offset.x = 0;
		_Scissor.offset.y = 0;
	}
	//_____________________________________________

	void vkGraphicsDevice::SetupViewport()
	{
		_Viewport.x = 0.0f;
		_Viewport.y = 0.0f;
		_Viewport.width = _size.m_Width;
		_Viewport.height = _size.m_Height;
		_Viewport.minDepth = 0.0f;
		_Viewport.maxDepth = 1.0f;
	}
	//_____________________________________________

	void vkGraphicsDevice::DrawFrame(float dt)
	{
		
		_worldMatrix = _worldMatrix * Core::Matrix44f::CreateRotateAroundY((90.f * (3.1415f / 180.f)) * dt);
		_worldMatrix = _worldMatrix * Core::Matrix44f::CreateRotateAroundX((45.f * (3.1415f / 180.f)) * dt);

		if (vkAcquireNextImageKHR(m_LogicalDevice->GetDevice(), m_Swapchain->GetSwapchain(), UINT64_MAX, m_AcquireNextImageSemaphore, VK_NULL_HANDLE/*fence*/, &m_Index) != VK_SUCCESS)
			assert(!"Failed to acquire next image!");


		/*void* vpData = nullptr;
		if (vkMapMemory(m_LogicalDevice->GetDevice(), _matrixBufferMemory, 0, sizeof(Core::Matrix44f) * 3, 0, &vpData) != VK_SUCCESS)
			assert(!"Failed to map memory!");

*/
		_ViewProjectionMatrix = _translationMatrix*_projectionMatrix;
		_ViewProjection.Bind(m_LogicalDevice->GetDevice(), 0);

/*
		int8* data = (int8*)vpData;
		memcpy(&data[0], &_worldMatrix, sizeof(Core::Matrix44f));
		memcpy(&data[sizeof(Core::Matrix44f) * 1], &_translationMatrix, sizeof(Core::Matrix44f));
		memcpy(&data[sizeof(Core::Matrix44f) * 2], &_projectionMatrix, sizeof(Core::Matrix44f));

		vkUnmapMemory(m_LogicalDevice->GetDevice(), _matrixBufferMemory);
*/
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
	//_____________________________________________


	void vkGraphicsDevice::CreateRenderPass()
	{

		VkAttachmentDescription attDesc = {};
		attDesc.format = m_Swapchain->GetFormat().format;
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

		VkSubpassDescription subpassDesc = {};
		subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDesc.colorAttachmentCount = 1;
		subpassDesc.pColorAttachments = &attRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo rpInfo = {};
		rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		rpInfo.attachmentCount = 1;
		rpInfo.pAttachments = &attDesc;
		rpInfo.subpassCount = 1;
		rpInfo.pSubpasses = &subpassDesc;
		rpInfo.dependencyCount = 1;
		rpInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(m_LogicalDevice->GetDevice(), &rpInfo, nullptr, &_renderPass) != VK_SUCCESS)
			assert(!"Failed to create renderpass");
	}
	//_____________________________________________

	void vkGraphicsDevice::CreateCommandPool()
	{
		VkCommandPoolCreateInfo poolCreateInfo = {};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolCreateInfo.pNext = nullptr;
		poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolCreateInfo.queueFamilyIndex = m_PhysicalDevice->GetQueueFamilyIndex();

		if (vkCreateCommandPool(m_LogicalDevice->GetDevice(), &poolCreateInfo, nullptr, &m_CmdPool) != VK_SUCCESS)
			assert(!"failed to create VkCommandPool!");
	}
	//_____________________________________________

	void vkGraphicsDevice::CreateCommandBuffer()
	{
		VkCommandBufferAllocateInfo cmdBufAllocInfo = {};
		cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocInfo.commandPool = m_CmdPool;
		cmdBufAllocInfo.commandBufferCount = (uint32)m_Swapchain->GetNofImages();
		cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		m_CmdBuffers.resize(m_Swapchain->GetNofImages());

		if (vkAllocateCommandBuffers(m_LogicalDevice->GetDevice(), &cmdBufAllocInfo, m_CmdBuffers.data()) != VK_SUCCESS)
			assert(!"Failed to create VkCommandBuffer!");
	}
	//_____________________________________________

	void vkGraphicsDevice::CreateGraphicsPipeline()
	{
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
		rastCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
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

		auto bindDesc = CreateBindDesc();
		auto attrDesc = CreateAttrDesc(0, 0);
		auto attrDesc2 = CreateAttrDesc(1, 16);

		VkVertexInputAttributeDescription descriptions[] = { attrDesc, attrDesc2 };


		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = 2;

		vertexInputInfo.pVertexBindingDescriptions = &bindDesc;
		vertexInputInfo.pVertexAttributeDescriptions = descriptions;

		CreateDescriptorLayout();
		CreateDescriptorPool();
		CreateDescriptorSet();

		VkDescriptorBufferInfo bInfo = {};
		bInfo.buffer = _ViewProjection.Get();
		bInfo.offset = 0;
		bInfo.range = sizeof(Core::Matrix44f) * 2;

		VkWriteDescriptorSet descWrite = {};
		descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descWrite.dstSet = _descriptorSet;
		descWrite.dstBinding = 0;
		descWrite.dstArrayElement = 0;
		descWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descWrite.descriptorCount = 1;
		descWrite.pBufferInfo = &bInfo;
		descWrite.pImageInfo = nullptr;
		descWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(m_LogicalDevice->GetDevice(), 1, &descWrite, 0, nullptr);

		VkPipelineInputAssemblyStateCreateInfo pipelineIACreateInfo = {};
		pipelineIACreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipelineIACreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipelineIACreateInfo.primitiveRestartEnable = VK_FALSE;

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
		pipelineInfo.stageCount = ARRSIZE(ssci);

		if (vkCreateGraphicsPipelines(m_LogicalDevice->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline) != VK_SUCCESS)
			assert(!"Failed to create pipeline!");
	}

	void vkGraphicsDevice::CreateDescriptorLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;


		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;
	

		if (vkCreateDescriptorSetLayout(m_LogicalDevice->GetDevice(), &layoutInfo, nullptr, &_descriptorLayout) != VK_SUCCESS)
			assert(!"Failed to create Descriptor layout");

	}

	void vkGraphicsDevice::CreateDescriptorPool()
	{

		VkDescriptorPoolSize poolSize = {};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = 1; // (uint32_t)m_Swapchain->GetNofImages();
		
		VkDescriptorPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.poolSizeCount = 1;
		createInfo.pPoolSizes = &poolSize;
		createInfo.maxSets = (uint32_t)m_Swapchain->GetNofImages();

		if (vkCreateDescriptorPool(m_LogicalDevice->GetDevice(), &createInfo, nullptr, &_descriptorPool) != VK_SUCCESS)
			assert(!"Failed to create descriptorPool");
	}

	void vkGraphicsDevice::CreateDescriptorSet()
	{
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _descriptorPool;
		allocInfo.descriptorSetCount = 1; // (uint32_t)m_Swapchain->GetNofImages();
		allocInfo.pSetLayouts = &_descriptorLayout;
		if (vkAllocateDescriptorSets(m_LogicalDevice->GetDevice(), &allocInfo, &_descriptorSet) != VK_SUCCESS)
			assert(!"failed to allocate descriptor sets!");
	}

	//_____________________________________________

	void vkGraphicsDevice::CreatePipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineCreateInfo.setLayoutCount = 1;
		pipelineCreateInfo.pSetLayouts = &_descriptorLayout;

		if (vkCreatePipelineLayout(m_LogicalDevice->GetDevice(), &pipelineCreateInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
			assert(!"Failed to create pipelineLayout");
	}
	//_____________________________________________

	void vkGraphicsDevice::CreateFramebuffers()
	{
		m_FrameBuffers.resize(m_Swapchain->GetNofImages());
		auto& list = m_Swapchain->GetImageList();
		auto& viewList = m_Swapchain->GetImageViewList();
		for (size_t i = 0; i < m_Swapchain->GetNofImages(); ++i)
		{
			VkImageViewCreateInfo vcInfo = {};
			vcInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			vcInfo.image = list[i];
			vcInfo.format = m_Swapchain->GetFormat().format;
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
				assert(!"Failed to create ImageView!");

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
	}

	uint32 findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags, VkPhysicalDevice device)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(device, &memProperties);


		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & propertyFlags ) == propertyFlags)
			{
				return i;
			}
		}
		assert(!"Failed to find suitable memory type!");
		return 0;
	}

	void vkGraphicsDevice::CreateVertexBuffer()
	{
		const int32 _dataSize = sizeof(_triangle);

		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = _dataSize;
		createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


		CreateBuffer(createInfo, &_VertexBuffer, &_vertexBufferMemory);

		void* data = nullptr;
		if (vkMapMemory(m_LogicalDevice->GetDevice(), _vertexBufferMemory, 0, _dataSize, 0, &data) != VK_SUCCESS)
			assert(!"Failed to map memory!");
		memcpy(data, _triangle, _dataSize);
		vkUnmapMemory(m_LogicalDevice->GetDevice(), _vertexBufferMemory);

	}

	void vkGraphicsDevice::CreateCube()
	{
		const int32 _dataSize = sizeof(_cube);

		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = _dataSize;
		createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


		CreateBuffer(createInfo, &_cubeBuffer, &_cubeBufferMemory);

		void* data = nullptr;
		if (vkMapMemory(m_LogicalDevice->GetDevice(), _cubeBufferMemory, 0, _dataSize, 0, &data) != VK_SUCCESS)
			assert(!"Failed to map memory!");
		memcpy(data, _cube, _dataSize);
		vkUnmapMemory(m_LogicalDevice->GetDevice(), _cubeBufferMemory);
	}

	VkVertexInputBindingDescription vkGraphicsDevice::CreateBindDesc()
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;		
	}

	VkVertexInputAttributeDescription vkGraphicsDevice::CreateAttrDesc(int location, int offset)
	{
		VkVertexInputAttributeDescription attrDesc = {};
		attrDesc.binding = 0;
		attrDesc.location = location;
		attrDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attrDesc.offset = offset;
		return attrDesc;
	}


	void vkGraphicsDevice::CreateMatrixBuffer()
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = sizeof(Core::Matrix44f) * 3;
		createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		CreateBuffer(createInfo, &_MatrixBuffer, &_matrixBufferMemory);		
		
	}

	void vkGraphicsDevice::CreateBuffer(const VkBufferCreateInfo& createInfo, VkBuffer* buffer, VkDeviceMemory* memory)
	{
		if (vkCreateBuffer(m_LogicalDevice->GetDevice(), &createInfo, nullptr, buffer) != VK_SUCCESS)
			assert(!"Failed to create vertex buffer!");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_LogicalDevice->GetDevice(), *buffer, &memRequirements);

		*memory = GPUAllocateMemory(memRequirements, m_LogicalDevice->GetDevice(), m_PhysicalDevice->GetDevice());

		if (vkBindBufferMemory(m_LogicalDevice->GetDevice(), *buffer, *memory, 0) != VK_SUCCESS)
			assert(!"Failed to bind buffer memory!");
	}

	VkSemaphore vkGraphicsDevice::CreateVkSemaphore(VkDevice pDevice)
	{
		VkSemaphore semaphore = nullptr;
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		if (vkCreateSemaphore(pDevice, &semaphoreCreateInfo, nullptr, &semaphore) != VK_SUCCESS)
			assert(!"Failed to create VkSemaphore");

		return semaphore;
	}
	//_____________________________________________

	VkShaderModule vkGraphicsDevice::LoadShader(const char* filepath, VkDevice pDevice)
	{
		Core::File shader(filepath, Core::FileMode::READ_FILE);
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shader.GetSize();
		createInfo.pCode = (const uint32_t*)shader.GetBuffer();

		VkShaderModule shaderModule = nullptr;
		if (vkCreateShaderModule(pDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			assert(!"Failed to create VkShaderModule!");

		return shaderModule;
	}
	//_____________________________________________

}; //namespace Graphics