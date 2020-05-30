#include "vkGraphicsDevice.h"

#include "Camera.h"

#include "VlkInstance.h"
#include "VlkPhysicalDevice.h"
#include "VlkDevice.h"
#include "VlkSwapchain.h"
#include "VlkSurface.h"

#include "Utilities.h"
#include "Window.h"

#include "Core/File.h"
#include "Core/math/Matrix44.h"
#include "Core/utilities/Randomizer.h"
#include "Input/InputManager.h"
#include "input/InputDeviceMouse_Win32.h"
#include "input/InputDeviceKeyboard_Win32.h"

#include "logger/Debug.h"

#include <windows.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_vulkan.h"
#include "imgui/examples/imgui_impl_win32.h"

VkRenderPass _renderPass = nullptr;

VkPipeline _pipeline = nullptr;
VkPipelineLayout _pipelineLayout = nullptr;
VkViewport _Viewport = {};
VkRect2D _Scissor = {};

VkDescriptorSetLayout _descriptorLayout = nullptr;
VkDescriptorPool _descriptorPool = nullptr;
VkDescriptorSet _descriptorSet = nullptr;

VkImage _depthImage = nullptr;
VkImageView _depthView = nullptr;
VkDeviceMemory _depthImageMemory = nullptr;

Graphics::Camera _Camera;

Window::Size _size;

Core::Vector4f _LightDir;
Core::Matrix44f _LightObject = Core::Matrix44f::Identity();

// Vertex Description
struct Vertex
{
	Core::Vector4f position;
	Core::Vector4f color;
	Core::Vector4f normal;
};

namespace Graphics
{
	struct VertexBuffer
	{
		VkBuffer m_Buffer;
		VkDeviceMemory m_Memory;
		int32 m_VertexCount = 0;
		int32 m_Stride = 0;
		int32 m_Offset = 0;
	};

	struct Cube
	{
		VertexBuffer m_VertexBuffer;
		Core::Matrix44f m_Orientation;

		void update(float /*dt*/)
		{
		}

		void Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
		{

			char* data = new char[sizeof(Core::Matrix44f)];
			memset(data, 0, sizeof(Core::Matrix44f));
			memcpy(&data[0], &m_Orientation, sizeof(Core::Matrix44f));
			// memcpy(&data[sizeof(Core::Matrix44f)], &_LightDir, sizeof(Core::Vector4f)); // This is not good at all

			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Core::Matrix44f), data);
			/* This is quite a strange one, this is only gonna be available in non-instanced entities for position */

			VkDeviceSize offset = 0;
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBuffer.m_Buffer, &offset);

			// This executes secondary commandBuffers inside of the primary commandBuffer
			// vkCmdExecuteCommands(commandBuffer, 0, nullptr);

			vkCmdDraw(commandBuffer, m_VertexBuffer.m_VertexCount, 1, 0, 0);

			delete[] data;
			data = nullptr;
		}

		void destroy(VkDevice device)
		{
			vkDestroyBuffer(device, m_VertexBuffer.m_Buffer, nullptr);
		}

		void init(VlkDevice* device, VlkPhysicalDevice* physicalDevice)
		{
			Core::File loader("cube.mdl", Core::File::READ_FILE);

			m_Orientation = Core::Matrix44f::Identity();
			m_VertexBuffer.m_Stride = sizeof(Vertex);
			m_VertexBuffer.m_VertexCount = (loader.GetSize() / sizeof(Vertex));
			m_VertexBuffer.m_Offset = 0;

			const int32 dataSize = m_VertexBuffer.m_Stride * m_VertexBuffer.m_VertexCount;

			VkBufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.size = dataSize;
			createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			m_VertexBuffer.m_Buffer = device->CreateBuffer(createInfo, &m_VertexBuffer.m_Memory, physicalDevice);

			void* data = nullptr;
			if(vkMapMemory(device->GetDevice(), m_VertexBuffer.m_Memory, 0, dataSize, 0, &data) != VK_SUCCESS)
				ASSERT(false, "Failed to map memory!");

			memcpy(data, loader.GetBuffer(), loader.GetSize());
			vkUnmapMemory(device->GetDevice(), m_VertexBuffer.m_Memory);
		}

		void setPosition(const Core::Vector4f& position)
		{
			m_Orientation.SetPosition(position);
		}
	};

	struct Shader
	{
		VkShaderModule m_Module = nullptr;
		void Create(VkShaderModule module)
		{
			m_Module = module;
		}
		VkShaderModule GetModule()
		{
			return m_Module;
		}
	};

	Shader _vertexShader;
	Shader _fragmentShader;

	std::vector<Cube> _Cubes;

	ConstantBuffer _ViewProjection;

	vkGraphicsDevice::vkGraphicsDevice() = default;

	vkGraphicsDevice::~vkGraphicsDevice()
	{

		auto device = m_LogicalDevice->GetDevice();
		DestroyConstantBuffer(&_ViewProjection);

		for(Cube& cube : _Cubes)
			cube.destroy(m_LogicalDevice->GetDevice());

		DestroyShader(&_vertexShader);
		DestroyShader(&_fragmentShader);

		vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);
		vkDestroyPipeline(device, _pipeline, nullptr);

		vkDestroySemaphore(device, m_DrawDone, nullptr);
		vkDestroySemaphore(device, m_AcquireNextImageSemaphore, nullptr);
		vkDestroyCommandPool(device, m_CmdPool, nullptr);

		for(VkFramebuffer buffer : m_FrameBuffers)
			vkDestroyFramebuffer(device, buffer, nullptr);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
		ImGui::DestroyContext();
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplVulkan_Shutdown();

		SAFE_DELETE(m_Swapchain);
		SAFE_DELETE(m_LogicalDevice);
		SAFE_DELETE(m_PhysicalDevice);
		SAFE_DELETE(m_Instance);
	}

	bool vkGraphicsDevice::Init(const Window& window)
	{
		_size = window.GetInnerSize();
		_Camera.InitPerspectiveProjection(_size.m_Width, _size.m_Height, 0.1f, 1000.f, 90.f);
		_Camera.SetTranslation({ 0.f, 0.f, -25.f, 1.f });

		m_Instance = new VlkInstance();
		m_Instance->Init();

		m_PhysicalDevice = new VlkPhysicalDevice();
		m_PhysicalDevice->Init(m_Instance);

		m_LogicalDevice = new VlkDevice();
		m_LogicalDevice->Init(m_PhysicalDevice);

		m_Swapchain = new VlkSwapchain();
		m_Swapchain->Init(m_Instance, m_LogicalDevice, m_PhysicalDevice, window);

		//_ViewProjection.RegVar(_Camera.GetView());
		//_ViewProjection.RegVar(_Camera.GetProjection());
		_ViewProjection.RegVar(_Camera.GetViewProjectionPointer());
		_ViewProjection.RegVar(&_LightDir);

		CreateConstantBuffer(&_ViewProjection);
		CreateCommandPool();

		// Create two commandBuffers and push into the array
		m_CmdBuffers.push_back(CreateCommandBuffer(m_LogicalDevice->GetDevice(), m_CmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY));
		m_CmdBuffers.push_back(CreateCommandBuffer(m_LogicalDevice->GetDevice(), m_CmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY));

		CreateDepthResources();
		_renderPass = CreateRenderPass();

		m_FrameBuffers.resize(m_Swapchain->GetNofImages());
		auto& list = m_Swapchain->GetImageList();
		auto& viewList = m_Swapchain->GetImageViewList();
		for(int i = 0; i < m_FrameBuffers.size(); i++)
		{
			viewList[i] = CreateImageView(m_Swapchain->GetFormat().format, list[i], VK_IMAGE_ASPECT_COLOR_BIT);

			VkImageView views[] = { viewList[i], _depthView };
			m_FrameBuffers[i] = CreateFramebuffer(views, ARRSIZE(views), window);
		}

		LoadShader(&_vertexShader, "Data/Shaders/vertex.vert");
		LoadShader(&_fragmentShader, "Data/Shaders/frag.hlsl");

		CreateViewport(0.f, 0.f, _size.m_Width, _size.m_Height, 0.f, 1.f, &_Viewport);
		SetupScissorArea((uint32)_size.m_Width, (uint32)_size.m_Height, 0, 0, &_Scissor);

		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.descriptorCount = 1;

		VkDescriptorSetLayoutBinding bindings[] = {
			uboLayoutBinding,
		};

		_descriptorLayout = CreateDescriptorLayout(bindings, ARRSIZE(bindings));

		VkDescriptorSetLayout descriptorLayouts[] = {
			_descriptorLayout,
		};

		VkPushConstantRange pushRangeList[] = {
			{ VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Core::Matrix44f) + sizeof(Core::Vector4f) },
		};

		_pipelineLayout = CreatePipelineLayout(descriptorLayouts, ARRSIZE(descriptorLayouts), pushRangeList, ARRSIZE(pushRangeList));
		_pipeline = CreateGraphicsPipeline();

		m_AcquireNextImageSemaphore = CreateVkSemaphore(m_LogicalDevice->GetDevice());
		m_DrawDone = CreateVkSemaphore(m_LogicalDevice->GetDevice());

		const float xValue = -22.f;
		const float yValue = -12.f;
		const float zValue = 0.f;
		Core::Vector4f position{ xValue, yValue, zValue, 1.f };

		for(int i = 0; i < 128; i++)
		{
			_Cubes.push_back(Cube());
			Cube& last = _Cubes.back();
			last.init(m_LogicalDevice, m_PhysicalDevice);
			last.setPosition(position);

			position.x += 5.f;
			if(i % 10 == 0 && i != 0)
			{
				position.y += 5.f;
				position.x = xValue;
			}
		}

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		if(vkCreateFence(m_LogicalDevice->GetDevice(), &fenceCreateInfo, nullptr, &m_CommandFence) != VK_SUCCESS)
			ASSERT(false, "Failed to create fence!");

		SetupImGui();

		return true;
	}
	//_____________________________________________

	void vkGraphicsDevice::SetupScissorArea(uint32 width, uint32 height, int32 offsetX, int32 offsetY, VkRect2D* scissorArea)
	{
		scissorArea->extent.width = width;
		scissorArea->extent.height = height;
		scissorArea->offset.x = offsetX;
		scissorArea->offset.y = offsetY;
	}

	//_____________________________________________

	void vkGraphicsDevice::DrawFrame(float dt)
	{

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();
		// static bool show_demo_window = true;
		// ImGui::ShowDemoWindow(&show_demo_window);

		ImGui::SetNextWindowSize(ImVec2(_size.m_Width * 0.15f, _size.m_Height * 0.25f));
		if(ImGui::Begin("blank", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			ImGui::Text("LightDir: X: %.3f Y: %.3f Z: %.3f", _LightDir.x, _LightDir.y, _LightDir.z);
			ImGui::End();
		}

		ImGui::Render();

		_LightObject = _LightObject * Core::Matrix44f::CreateRotateAroundX(Core::DegreeToRad(45.f) * dt);
		_LightDir = _LightObject.GetForward();

		if(vkAcquireNextImageKHR(m_LogicalDevice->GetDevice(), m_Swapchain->GetSwapchain(), UINT64_MAX, m_AcquireNextImageSemaphore, VK_NULL_HANDLE /*fence*/, &m_Index) !=
		   VK_SUCCESS)
			ASSERT(false, "Failed to acquire next image!");

		Input::InputManager& input = Input::InputManager::Get();

		Input::HInputDeviceMouse* mouse = nullptr;
		input.GetDevice(Input::EDeviceType_Mouse, &mouse);

		const Input::Cursor& cursor = mouse->GetCursor();

		Input::HInputDeviceKeyboard* keyboard = nullptr;
		input.GetDevice(Input::EDeviceType_Keyboard, &keyboard);
		const float speed = 10.f;

		if(keyboard->IsDown(DIK_W))
			_Camera.Forward(speed * dt);
		if(keyboard->IsDown(DIK_S))
			_Camera.Forward(-speed * dt);

		if(keyboard->IsDown(DIK_D))
			_Camera.Right(speed * dt);
		if(keyboard->IsDown(DIK_A))
			_Camera.Right(-speed * dt);

		if(keyboard->IsDown(DIK_R))
			_Camera.Up(speed * dt);
		if(keyboard->IsDown(DIK_F))
			_Camera.Up(-speed * dt);

		if(mouse->IsDown(1))
		{
			_Camera.OrientCamera({ cursor.dx, cursor.dy });
		}

		_Camera.Update();
		BindConstantBuffer(&_ViewProjection, 0);

		const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // associated with
																									 // having
																									 // semaphores
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitDstStageMask = &waitDstStageMask;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CmdBuffers[m_Index];

		submitInfo.pSignalSemaphores = &m_DrawDone;
		submitInfo.signalSemaphoreCount = 1;

		submitInfo.pWaitSemaphores = &m_AcquireNextImageSemaphore;
		submitInfo.waitSemaphoreCount = 1;

		// This line fails when running with renderdoc, suspecting empty queue would be the issue
		if(vkQueueSubmit(m_LogicalDevice->GetQueue(), 1, &submitInfo, m_CommandFence) != VK_SUCCESS)
			ASSERT(false, "Failed to submit the queue!");

		VkSwapchainKHR swapchain = m_Swapchain->GetSwapchain();

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &m_Index;
		presentInfo.pWaitSemaphores = &m_DrawDone;
		presentInfo.waitSemaphoreCount = 1;

		if(vkQueuePresentKHR(m_LogicalDevice->GetQueue(), &presentInfo) != VK_SUCCESS)
			ASSERT(false, "Failed to present!");

		for(Cube& cube : _Cubes)
		{
			cube.update(dt);
		}

		SetupRenderCommands(m_Index ^ 1);
	}

	//_____________________________________________

	VkRenderPass vkGraphicsDevice::CreateRenderPass()
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

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = findDepthFormat();
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

		VkRenderPass renderpass = nullptr;
		if(vkCreateRenderPass(m_LogicalDevice->GetDevice(), &rpInfo, nullptr, &renderpass) != VK_SUCCESS)
			ASSERT(false, "Failed to create renderpass");

		return renderpass;
	}
	//_____________________________________________

	void vkGraphicsDevice::CreateCommandPool()
	{
		VkCommandPoolCreateInfo poolCreateInfo = {};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolCreateInfo.pNext = nullptr;
		poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolCreateInfo.queueFamilyIndex = m_PhysicalDevice->GetQueueFamilyIndex();

		if(vkCreateCommandPool(m_LogicalDevice->GetDevice(), &poolCreateInfo, nullptr, &m_CmdPool) != VK_SUCCESS)
			ASSERT(false, "failed to create VkCommandPool!");
	}
	//_____________________________________________

	VkCommandBuffer vkGraphicsDevice::CreateCommandBuffer(VkDevice device, VkCommandPool pool, VkCommandBufferLevel bufferLevel)
	{
		VkCommandBufferAllocateInfo cmdBufAllocInfo = {};
		cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocInfo.commandPool = pool;
		cmdBufAllocInfo.commandBufferCount = 1; //(uint32)m_Swapchain->GetNofImages(); // This is just about the nof command buffers to be created.
		cmdBufAllocInfo.level = bufferLevel;	// VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Type of commandBuffer

		VkCommandBuffer buffer = nullptr;
		if(vkAllocateCommandBuffers(device, &cmdBufAllocInfo, &buffer) != VK_SUCCESS)
			ASSERT(false, "Failed to create VkCommandBuffer!");

		return buffer;
	}
	//_____________________________________________

	VkPipeline vkGraphicsDevice::CreateGraphicsPipeline()
	{
		// viewport
		VkPipelineViewportStateCreateInfo vpCreateInfo = {};
		vpCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		vpCreateInfo.viewportCount = 1;
		vpCreateInfo.scissorCount = 1;
		vpCreateInfo.pScissors = &_Scissor;
		vpCreateInfo.pViewports = &_Viewport;

		// depth
		VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {};
		pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
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
		rastCreateInfo.polygonMode = VK_POLYGON_MODE_FILL; // VK_POLYGON_MODE_LINE // Wireframe
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

		// Input Assembler
		auto bindDesc = CreateBindDesc();	 // Define the size of the input assembler
		auto attrDesc = CreateAttrDesc(0, 0); // Define each attribute of the input assembler
		auto attrDesc2 = CreateAttrDesc(1, 16);
		auto attrDesc3 = CreateAttrDesc(2, 32);
		// This is 100% based on the vertex and not something that should be manually setup.
		// Vertex Description should be on the model

		VkVertexInputBindingDescription bindDescArr[] = { bindDesc };
		VkVertexInputAttributeDescription descriptions[] = { attrDesc, attrDesc2, attrDesc3 };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		vertexInputInfo.vertexBindingDescriptionCount = ARRSIZE(bindDescArr);
		vertexInputInfo.vertexAttributeDescriptionCount = ARRSIZE(descriptions);

		vertexInputInfo.pVertexBindingDescriptions = bindDescArr;
		vertexInputInfo.pVertexAttributeDescriptions = descriptions;

		CreateDescriptorPool();
		CreateDescriptorSet();

		VkDescriptorBufferInfo bInfo2 = {};
		bInfo2.buffer = static_cast<VkBuffer>(_ViewProjection.GetBuffer());
		bInfo2.offset = 0;
		bInfo2.range = _ViewProjection.GetSize();

		VkWriteDescriptorSet descWrite = {};
		descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descWrite.dstSet = _descriptorSet;
		descWrite.dstBinding = 0;
		descWrite.dstArrayElement = 0;
		descWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descWrite.descriptorCount = 1;
		descWrite.pBufferInfo = &bInfo2;

		vkUpdateDescriptorSets(m_LogicalDevice->GetDevice(), 1, &descWrite, 0, nullptr);

		VkPipelineInputAssemblyStateCreateInfo pipelineIACreateInfo = {};
		pipelineIACreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipelineIACreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipelineIACreateInfo.primitiveRestartEnable = VK_FALSE;

		// the entry point of shader cannot be defined like this.
		VkPipelineShaderStageCreateInfo ssci[] = { CreateShaderStageInfo(VK_SHADER_STAGE_VERTEX_BIT, _vertexShader.GetModule(), "main"),
												   CreateShaderStageInfo(VK_SHADER_STAGE_FRAGMENT_BIT, _fragmentShader.GetModule(), "main") };

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

		VkPipeline pipeline;
		if(vkCreateGraphicsPipelines(m_LogicalDevice->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
			ASSERT(false, "Failed to create pipeline!");

		DestroyShader(&_vertexShader);
		DestroyShader(&_fragmentShader);

		return pipeline;
	}

	VkDescriptorSetLayout vkGraphicsDevice::CreateDescriptorLayout(VkDescriptorSetLayoutBinding* descriptorBindings, int32 bindingCount)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = bindingCount;
		layoutInfo.pBindings = descriptorBindings;
		VkDescriptorSetLayout descriptorLayout;
		if(vkCreateDescriptorSetLayout(m_LogicalDevice->GetDevice(), &layoutInfo, nullptr, &descriptorLayout) != VK_SUCCESS)
			ASSERT(false, "Failed to create Descriptor layout");

		return descriptorLayout;
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

		if(vkCreateDescriptorPool(m_LogicalDevice->GetDevice(), &createInfo, nullptr, &_descriptorPool) != VK_SUCCESS)
			ASSERT(false, "Failed to create descriptorPool");
	}

	void vkGraphicsDevice::CreateDescriptorSet()
	{
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _descriptorPool;
		allocInfo.descriptorSetCount = 1; // (uint32_t)m_Swapchain->GetNofImages();

		VkDescriptorSetLayout layouts[] = {
			_descriptorLayout,
		};

		allocInfo.pSetLayouts = layouts;

		if(vkAllocateDescriptorSets(m_LogicalDevice->GetDevice(), &allocInfo, &_descriptorSet) != VK_SUCCESS)
			ASSERT(false, "failed to allocate descriptor sets!");
	}

	//_____________________________________________

	VkPipelineLayout vkGraphicsDevice::CreatePipelineLayout(VkDescriptorSetLayout* descriptorLayouts, int32 descriptorLayoutCount, VkPushConstantRange* pushConstantRange,
															int32 pushConstantRangeCount)
	{
		VkPipelineLayoutCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		pipelineCreateInfo.setLayoutCount = descriptorLayoutCount;
		pipelineCreateInfo.pSetLayouts = descriptorLayouts;

		pipelineCreateInfo.pushConstantRangeCount = pushConstantRangeCount;
		pipelineCreateInfo.pPushConstantRanges = pushConstantRange;

		VkPipelineLayout pipeline;
		if(vkCreatePipelineLayout(m_LogicalDevice->GetDevice(), &pipelineCreateInfo, nullptr, &pipeline) != VK_SUCCESS)
			ASSERT(false, "Failed to create pipelineLayout");
		return pipeline;
	}
	//_____________________________________________

	VkImageView vkGraphicsDevice::CreateImageView(VkFormat format, VkImage image, VkImageAspectFlags aspectFlag)
	{
		// This is a logic device operation

		VkImageViewCreateInfo vcInfo = {};
		vcInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		vcInfo.image = image;
		vcInfo.format = format;
		vcInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		vcInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
		VkImageSubresourceRange& srr = vcInfo.subresourceRange;
		srr.aspectMask = aspectFlag;
		srr.baseMipLevel = 0;
		srr.levelCount = 1;
		srr.baseArrayLayer = 0;
		srr.layerCount = 1;
		VkImageView view;
		if(vkCreateImageView(m_LogicalDevice->GetDevice(), &vcInfo, nullptr, &view) != VK_SUCCESS)
			ASSERT(false, "Failed to create ImageView!");

		return view;
	}

	// This is wrong, this is not how a frame buffer should be created. It should take a width & height not a window
	VkFramebuffer vkGraphicsDevice::CreateFramebuffer(VkImageView* view, int32 attachmentCount, const Window& window)
	{
		// This is a logic device operation
		VkFramebufferCreateInfo ci = {};
		ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		ci.renderPass = _renderPass;
		ci.attachmentCount = attachmentCount;
		ci.pAttachments = view;
		ci.width = (uint32)window.GetInnerSize().m_Width;
		ci.height = (uint32)window.GetInnerSize().m_Height;
		ci.layers = 1;

		VkFramebuffer framebuffer;
		if(vkCreateFramebuffer(m_LogicalDevice->GetDevice(), &ci, nullptr, &framebuffer) != VK_SUCCESS)
			ASSERT(false, "Failed to create framebuffer!");
		return framebuffer;
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

	VkSemaphore vkGraphicsDevice::CreateVkSemaphore(VkDevice pDevice)
	{
		VkSemaphore semaphore = nullptr;
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		if(vkCreateSemaphore(pDevice, &semaphoreCreateInfo, nullptr, &semaphore) != VK_SUCCESS)
			ASSERT(false, "Failed to create VkSemaphore");

		return semaphore;
	}
	//_____________________________________________

	VkShaderModule vkGraphicsDevice::LoadShader(const char* filepath, VkDevice pDevice)
	{
		Core::File shader(filepath, Core::File::READ_FILE);
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shader.GetSize();
		createInfo.pCode = (const uint32_t*)shader.GetBuffer();

		VkShaderModule shaderModule = nullptr;
		if(vkCreateShaderModule(pDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			ASSERT(false, "Failed to create VkShaderModule!");

		return shaderModule;
	}

	void vkGraphicsDevice::LoadShader(HShader* shader, const char* filepath)
	{
		shader->Create(LoadShader(filepath, m_LogicalDevice->GetDevice()));
	}

	void vkGraphicsDevice::BindConstantBuffer(ConstantBuffer* constantBuffer, uint32 offset)
	{
		auto lDevice = m_LogicalDevice->GetDevice();

		VkDeviceMemory deviceMem = static_cast<VkDeviceMemory>(constantBuffer->GetDeviceMemory());
		void* data = nullptr;
		if(vkMapMemory(lDevice, deviceMem, offset, constantBuffer->GetSize(), 0, &data) != VK_SUCCESS)
			ASSERT(false, "Failed to map memory!");

		int8* pMem = static_cast<int8*>(data);
		int32 step = 0;
		for(auto it : constantBuffer->GetVariables())
		{
			memcpy(&pMem[step], static_cast<int8*>(it.var), it.size);
			step += it.size;
		}
		vkUnmapMemory(lDevice, deviceMem);
	}

	void vkGraphicsDevice::DestroyConstantBuffer(ConstantBuffer* constantBuffer)
	{
		vkDestroyBuffer(m_LogicalDevice->GetDevice(), static_cast<VkBuffer>(constantBuffer->GetBuffer()), nullptr);
	}

	//_____________________________________________

	void vkGraphicsDevice::CreateConstantBuffer(ConstantBuffer* constantBuffer)
	{
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

		VkDeviceMemory deviceMem = m_LogicalDevice->AllocateMemory(memRequirements, m_PhysicalDevice);

		if(vkBindBufferMemory(lDevice, vulkan_buffer, deviceMem, 0) != VK_SUCCESS)
			ASSERT(false, "Failed to bind buffer memory!");

		constantBuffer->SetBuffer(vulkan_buffer);
		constantBuffer->SetDeviceMemory(deviceMem);
	}

	void vkGraphicsDevice::SetupRenderCommands(int index)
	{
		vkWaitForFences(m_LogicalDevice->GetDevice(), 1, &m_CommandFence, VK_TRUE, UINT64_MAX);
		vkResetFences(m_LogicalDevice->GetDevice(), 1, &m_CommandFence);

		VkCommandBufferBeginInfo cmdInfo = {};
		cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		VkFramebuffer& frameBuffer = m_FrameBuffers[index];
		VkCommandBuffer& commandBuffer = m_CmdBuffers[index];

		if(vkBeginCommandBuffer(commandBuffer, &cmdInfo) != VK_SUCCESS)
			ASSERT(false, "Failed to begin CommandBuffer!");

		VkRenderPassBeginInfo pass_info = {};
		PrepareRenderPass(&pass_info, frameBuffer, _size.m_Width, _size.m_Height);

		vkCmdBeginRenderPass(commandBuffer, &pass_info, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_descriptorSet, 0, nullptr);

		/* This thing right here is what I'm looking for */

		for(Cube& cube : _Cubes)
		{
			cube.Draw(commandBuffer, _pipelineLayout);
		}

		/* This is what draws the cubes */

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

		vkCmdEndRenderPass(commandBuffer);

		if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			ASSERT(false, "Failed to end CommandBuffer!");
	}

	void vkGraphicsDevice::DestroyShader(HShader* pShader)
	{
		vkDestroyShaderModule(m_LogicalDevice->GetDevice(), pShader->GetModule(), nullptr);
	}

	Camera* vkGraphicsDevice::GetCamera()
	{
		return &_Camera;
	}

	VkFormat vkGraphicsDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for(VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice->GetDevice(), format, &props);

			if(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if(tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		ASSERT(false, "undefined format!");
		return VK_FORMAT_UNDEFINED;
	}

	VkFormat vkGraphicsDevice::findDepthFormat()
	{
		return findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL,
								   VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	bool hasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void vkGraphicsDevice::CreateDepthResources()
	{
		VkFormat depthFormat = findDepthFormat();
		const VkExtent2D extent = m_Swapchain->GetExtent();
		CreateImage(extent.width, extent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					_depthImage, _depthImageMemory);
		_depthView = CreateImageView(depthFormat, _depthImage, VK_IMAGE_ASPECT_DEPTH_BIT);

		transitionImageLayout(_depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	uint32_t vkGraphicsDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice->GetDevice(), &memProperties);

		for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void vkGraphicsDevice::CreateImage(uint32 width, uint32 height, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
									   VkImage& image, VkDeviceMemory& imageMemory)
	{
		VkDevice device = m_LogicalDevice->GetDevice();

		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = imageTiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if(vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if(vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, image, imageMemory, 0);
	}

	// Need to look into what this function actually does
	void vkGraphicsDevice::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;

		if(newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if(hasStencilComponent(format))
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

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

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
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		// vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		endSingleTimeCommands(commandBuffer);
	}

	// Util Function
	void vkGraphicsDevice::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);
	}

	// Extends a commandbuffer
	VkCommandBuffer vkGraphicsDevice::beginSingleTimeCommands()
	{
		VkDevice device = m_LogicalDevice->GetDevice();

		VkCommandBuffer buffer;
		buffer = CreateCommandBuffer(device, m_CmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VERIFY(vkBeginCommandBuffer(buffer, &beginInfo) == VK_SUCCESS, "vkBeginCommandBuffer failed!");

		return buffer;
	}

	void vkGraphicsDevice::endSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_LogicalDevice->GetQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_LogicalDevice->GetQueue());

		vkFreeCommandBuffers(m_LogicalDevice->GetDevice(), m_CmdPool, 1, &commandBuffer);
	}

	VkPipelineShaderStageCreateInfo vkGraphicsDevice::CreateShaderStageInfo(VkShaderStageFlagBits stageFlags, VkShaderModule module, const char* entryPoint)
	{
		VkPipelineShaderStageCreateInfo stageInfo = {};
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.stage = stageFlags;
		stageInfo.module = module;
		stageInfo.pName = entryPoint;
		return stageInfo;
	}

	void vkGraphicsDevice::CreateViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth, VkViewport* viewport)
	{
		viewport->x = topLeftX;
		viewport->y = topLeftY;
		viewport->width = width;
		viewport->height = height;
		viewport->minDepth = minDepth;
		viewport->maxDepth = maxDepth;
	}

	void vkGraphicsDevice::SetupImGui()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize.x = _size.m_Width;
		io.DisplaySize.y = _size.m_Height;

		VkDevice lDevice = m_LogicalDevice->GetDevice();
		VkPhysicalDevice pDevice = m_PhysicalDevice->GetDevice();
		VkInstance instance = m_Instance->GetVKInstance();
		VkQueue queue = m_LogicalDevice->GetQueue();
		uint32 queueFamily = m_PhysicalDevice->GetQueueFamilyIndex();

		ImGui_ImplVulkan_InitInfo info = {};
		info.Device = lDevice;
		info.PhysicalDevice = pDevice;
		info.Instance = instance;
		info.Queue = queue;
		info.QueueFamily = queueFamily;
		info.DescriptorPool = _descriptorPool;
		info.MinImageCount = 2;
		info.ImageCount = m_Swapchain->GetNofImages();

		if(!ImGui_ImplVulkan_Init(&info, _renderPass))
			ASSERT(false, "Failed");

		if(vkResetCommandPool(lDevice, m_CmdPool, 0) != VK_SUCCESS)
			ASSERT(false, "failed to reset commandPool");

		VkCommandBuffer command_buffer = beginSingleTimeCommands();

		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

		endSingleTimeCommands(command_buffer);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	void vkGraphicsDevice::PrepareRenderPass(VkRenderPassBeginInfo* pass_info, VkFramebuffer framebuffer, uint32 width, uint32 height)
	{
		static VkClearValue clearValue[2] = {};
		clearValue[0].color = { 0.f, 0.f, 0.f, 0.f };
		clearValue[1].depthStencil = { 1.f, 0 };
		pass_info->sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		pass_info->renderPass = _renderPass;
		pass_info->renderArea.offset = { 0, 0 };
		pass_info->renderArea.extent = { width, height };
		pass_info->clearValueCount = ARRSIZE(clearValue);
		pass_info->pClearValues = clearValue;
		pass_info->framebuffer = framebuffer;
	}

}; // namespace Graphics
