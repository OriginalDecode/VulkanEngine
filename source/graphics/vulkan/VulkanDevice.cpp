#include "VulkanDevice.h"

#include "logger/Debug.h"

#include "graphics/GraphicsEngine.h"
#include "graphics/Window.h"

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include "thirdparty/imgui/imgui.h"

#include <vulkan/vulkan.h>
#ifdef _WIN32
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#include "graphics/imgui/imgui_impl_win32.h"
#endif
#include "graphics/imgui/imgui_impl_vulkan.h"

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT /* flags */,
												   VkDebugReportObjectTypeEXT /* objectType */,
												   uint64_t /* object */, size_t /* location */,
												   int32_t /* messageCode */,
												   const char* /* pLayerPrefix */,
												   const char* pMessage, void* /* pUserData */)
{
	char temp[USHRT_MAX] = { 0 };
	sprintf_s(temp, "Vulkan Warning :%s", pMessage);
	OutputDebugStringA(pMessage);
	OutputDebugStringA("\n");
	LOG_MESSAGE(temp);
	// assert( false && temp );
	return VK_FALSE;
}

#define VK_GET_FNC_POINTER(function, instance) \
	(PFN_##function) vkGetInstanceProcAddr(instance, #function)

namespace Graphics
{
	VkDebugReportCallbackEXT debugCallback = nullptr;

	VulkanDevice::~VulkanDevice()
	{

		ImGui::DestroyContext();
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplVulkan_Shutdown();

		for(int32 i = 0; i < m_NofSwapchainImages; ++i)
		{
			vkDestroyImage(m_Device, m_Images[i], nullptr);
			vkDestroyImageView(m_Device, m_ImageViews[i], nullptr);
		}

		vkDestroyImage(m_Device, m_DefaultDepthImage, nullptr);
		vkDestroyImageView(m_Device, m_DefaultDepthImageView, nullptr);

		delete[] m_Images;
		m_Images = nullptr;

		delete[] m_ImageViews;
		m_ImageViews = nullptr;

		auto destoryer = VK_GET_FNC_POINTER(vkDestroyDebugReportCallbackEXT, m_Instance);
		destoryer(m_Instance, debugCallback, nullptr);

		vkDestroyInstance(m_Instance, nullptr);
		vkDestroyDevice(m_Device, nullptr);
	}

	void VulkanDevice::Init()
	{
		CreateInstance();
		CreatePhysicalDevice();
		CreateDevice();
		CreateSwapchainSurface();
		CreateSwapchain();
	}

	void VulkanDevice::CreateBuffer(const VkBufferCreateInfo& create_info, VkBuffer* buffer)
	{
		if(vkCreateBuffer(m_Device, &create_info, nullptr, buffer) != VK_SUCCESS)
		{
			ASSERT(false, "failed to create vkBuffer!");
		}
	}

	VkDeviceMemory VulkanDevice::AllocMemory(VkMemoryRequirements memory_requirements,
											 uint32 memory_type_index)
	{
		VkDeviceMemory memory = nullptr;
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memory_requirements.size;
		allocInfo.memoryTypeIndex = memory_type_index;

		if(vkAllocateMemory(m_Device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to allocate memory on GPU");
		}

		return memory;
	}

	VkDescriptorSet VulkanDevice::AllocDescriptorSet(VkDescriptorPool descPool, uint32 descCount,
													 VkDescriptorSetLayout* layouts)
	{
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descPool;
		allocInfo.descriptorSetCount = descCount;
		allocInfo.pSetLayouts = layouts;

		VkDescriptorSet descSet = nullptr;
		if(vkAllocateDescriptorSets(m_Device, &allocInfo, &descSet) != VK_SUCCESS)
			ASSERT(false, "failed to allocate descriptor sets!");

		return descSet;
	}

	void VulkanDevice::BindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, int32 offset)
	{
		if(vkBindBufferMemory(m_Device, buffer, memory, offset))
		{
			ASSERT(false, "failed to bind buffer memory!");
		}
	}

	void VulkanDevice::BindImageMemory(VkImage image, VkDeviceMemory memory, int32 offset)
	{
		if(vkBindImageMemory(m_Device, image, memory, offset))
		{
			ASSERT(false, "failed to bind image memory!");
		}
	}

	uint32 VulkanDevice::FindMemoryType(uint32 type_filter, VkMemoryPropertyFlags flags)
	{
		VkPhysicalDeviceMemoryProperties properties = {};
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &properties);
		for(uint32 i = 0; i < properties.memoryTypeCount; ++i)
		{
			if((type_filter & (1 << i)) &&
			   (properties.memoryTypes[i].propertyFlags & flags) == flags)
			{
				return i;
			}
		}

		ASSERT(false, " failed to find suitable memory type!");
		return 0;
	}

	void VulkanDevice::FreeCommandBuffers(VkCommandPool command_pool, VkCommandBuffer* buffers,
										  uint32 nof_buffers)
	{
		vkFreeCommandBuffers(m_Device, command_pool, nof_buffers, buffers);
	}

	std::tuple<VkImage, VkDeviceMemory>
		VulkanDevice::Create2DImage(uint32 width, uint32 height, VkFormat format,
									VkImageTiling image_tilig, VkImageUsageFlags usage_flags,
									VkMemoryPropertyFlags memory_flags)
	{
		VkImageCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		create_info.imageType = VK_IMAGE_TYPE_2D;
		create_info.extent.width = width;
		create_info.extent.height = height;
		create_info.extent.depth = 1;
		create_info.mipLevels = 1;
		create_info.arrayLayers = 1;
		create_info.format = format;
		create_info.tiling = image_tilig;
		create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		create_info.usage = usage_flags;
		create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkImage image;
		if(vkCreateImage(m_Device, &create_info, nullptr, &image) != VK_SUCCESS)
			ASSERT(false, "failed to create image!");

		VkMemoryRequirements memory_requirements = {};
		vkGetImageMemoryRequirements(m_Device, image, &memory_requirements);

		VkDeviceMemory memory = AllocMemory(
			memory_requirements, FindMemoryType(memory_requirements.memoryTypeBits, memory_flags));

		// BindImageMemory( image, memory, 0 );
		return std::make_tuple(image, memory);
	}

	VkImageView VulkanDevice::Create2DImageView(VkFormat format, VkImage image,
												VkImageAspectFlags flags)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = image;
		create_info.format = format;
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
								   VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
		VkImageSubresourceRange& srr = create_info.subresourceRange;
		srr.aspectMask = flags;
		srr.baseMipLevel = 0;
		srr.levelCount = 1;
		srr.baseArrayLayer = 0; // related to 3d type
		srr.layerCount = 1;		// related to 3d type

		VkImageView view = nullptr;
		if(vkCreateImageView(m_Device, &create_info, nullptr, &view) != VK_SUCCESS)
			ASSERT(false, "Failed to create ImageView!");

		return view;
	}

	VkFramebuffer VulkanDevice::CreateFramebuffer(VkImageView* image_views, uint32 nof_attachments,
												  uint32 width, uint32 height,
												  VkRenderPass renderpass)
	{
		VkFramebufferCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		create_info.renderPass = renderpass;
		create_info.attachmentCount = nof_attachments;
		create_info.pAttachments = image_views;
		create_info.width = width;
		create_info.height = height;
		create_info.layers = 1;

		VkFramebuffer framebuffer = nullptr;
		if(vkCreateFramebuffer(m_Device, &create_info, nullptr, &framebuffer) != VK_SUCCESS)
			ASSERT(false, "Failed to create framebuffer!");

		return framebuffer;
	}

	void VulkanDevice::CreateCommandBuffers(VkCommandPool command_pool, VkCommandBuffer* buffers,
											uint32 nof_buffers)
	{
		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = command_pool;
		alloc_info.commandBufferCount = nof_buffers;

		if(vkAllocateCommandBuffers(m_Device, &alloc_info, buffers) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to create VkCommandBuffers!");
			delete[] buffers;
			buffers = nullptr;
		}
	}

	VkCommandPool VulkanDevice::CreateCommandPool()
	{
		VkCommandPool commandPool = nullptr;
		VkCommandPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		create_info.pNext = nullptr;

		/* this is the only thing that can change */
		create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		create_info.queueFamilyIndex = m_QueueFamily;
		if(vkCreateCommandPool(m_Device, &create_info, nullptr, &commandPool) != VK_SUCCESS)
			ASSERT(false, "failed to create VkCommandPool!");

		return commandPool;
	}

	void VulkanDevice::CreateInstance()
	{
		VkApplicationInfo appInfo = {};

		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "wce engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "wce engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_1;

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;

		const char* validationLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
		instanceCreateInfo.enabledLayerCount = ARRSIZE(validationLayers);
		instanceCreateInfo.ppEnabledLayerNames = &validationLayers[0];

		const char* extentions[] = { "VK_KHR_surface", "VK_KHR_win32_surface",
									 "VK_EXT_debug_report" };
		instanceCreateInfo.enabledExtensionCount = ARRSIZE(extentions);
		instanceCreateInfo.ppEnabledExtensionNames = extentions;

		if(vkCreateInstance(&instanceCreateInfo, nullptr /*allocator*/, &m_Instance) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to create Vulkan instance!");
		}

		auto create = VK_GET_FNC_POINTER(vkCreateDebugReportCallbackEXT, m_Instance);
		ASSERT(create, "Failed to create callback function!");

		VkDebugReportFlagsEXT flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
									  VK_DEBUG_REPORT_WARNING_BIT_EXT |
									  VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;

		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.pfnCallback = &DebugReportCallback;
		createInfo.flags = flags;

		if(create(m_Instance, &createInfo, nullptr, &debugCallback) != VK_SUCCESS)
			ASSERT(false, "Failed to create vulkan debug callback!");
	}

	void VulkanDevice::CreatePhysicalDevice()
	{
		uint32 device_count = 0;
		if(vkEnumeratePhysicalDevices(m_Instance, &device_count, nullptr) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to enumerate gpus!");
		}

		VkPhysicalDevice* gpus = new VkPhysicalDevice[device_count];

		if(vkEnumeratePhysicalDevices(m_Instance, &device_count, gpus) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to enumerate device!");
		}

		int32 queueIndex = 0;
		VkQueueFamilyProperties* properties = nullptr;
		for(int32 i = 0; i < device_count; ++i)
		{
			VkPhysicalDevice device = gpus[i];
			uint32 property_count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &property_count, nullptr);
			properties = new VkQueueFamilyProperties[property_count];
			vkGetPhysicalDeviceQueueFamilyProperties(device, &property_count, properties);

			for(int32 j = 0; j < property_count; ++j)
			{
				VkQueueFamilyProperties property = properties[j];

				if(property.queueCount > 0 && property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					queueIndex = j;
					break;
				}
			}

			if(queueIndex > -1)
			{
				m_PhysicalDevice = device;
				m_QueueFamily = queueIndex;
				break;
			}
		}

		delete[] properties;
		delete[] gpus;

		ASSERT(m_PhysicalDevice, "No physical device after creation!");
	}

	void VulkanDevice::CreateDevice()
	{
		// queue create info
		const float queue_priorities[] = { 1.f };
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = m_QueueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = queue_priorities;

		// Physical device features
		VkPhysicalDeviceFeatures enabled_features = {};
		enabled_features.shaderClipDistance = true;

		// device create info
		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
#ifdef _DEBUG
		const char* debugLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
		createInfo.enabledLayerCount = ARRSIZE(debugLayers);
		createInfo.ppEnabledLayerNames = debugLayers;
#endif

		const char* deviceExt[] = { "VK_KHR_swapchain" };
		createInfo.enabledExtensionCount = ARRSIZE(deviceExt);
		createInfo.ppEnabledExtensionNames = deviceExt;
		createInfo.pEnabledFeatures = &enabled_features;

		if(vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to create device!");
		}

		ASSERT(m_Device, "Failed to create device!");

		vkGetDeviceQueue(m_Device, m_QueueFamily, 0 /*queueIndex*/, &m_Queue);
	}

	void VulkanDevice::CreateSwapchain()
	{

		int32 queueFamily;
		int32 queueIndex;

		VkQueueFamilyProperties* properties = nullptr;

		uint32 property_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &property_count, nullptr);
		properties = new VkQueueFamilyProperties[property_count];
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &property_count, properties);

		for(int32 i = 0; i < property_count; ++i)
		{
			VkQueueFamilyProperties property = properties[i];

			if(property.queueCount > 0)
			{
				if(SurfaceCanPresent(m_Surface))
				{
					queueFamily = i;
				}
				else
				{
					ASSERT(false, "surface can't present!?");
				}

				if(property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					queueIndex = i;
				}
				break;
			}
		}
		delete[] properties;
		properties = nullptr;

		VkSurfaceCapabilitiesKHR surface_capabilities = GetSurfaceCapabilities(m_Surface);

		uint32 image_count = 2;
		ASSERT(image_count < surface_capabilities.minImageCount, "");

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = surface_capabilities.minImageCount;

		VkSurfaceFormatKHR format = GetPhysicalDeviceSurfaceFormat();

		createInfo.imageFormat = format.format;
		createInfo.imageColorSpace = format.colorSpace;

		createInfo.imageExtent = surface_capabilities.currentExtent;
		createInfo.imageArrayLayers = 1;

		if(queueIndex != queueFamily)
		{
			const uint32_t queueIndices[] = { (uint32_t)queueIndex, (uint32_t)queueFamily };
			createInfo.queueFamilyIndexCount = ARRSIZE(queueIndices);
			createInfo.pQueueFamilyIndices = queueIndices;
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		}
		else
		{
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		VkSurfaceTransformFlagBitsKHR transformFlags = surface_capabilities.currentTransform;
		if(surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
			transformFlags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

		createInfo.preTransform = transformFlags;

		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; // this field
																// defines
																// blocking or
																// nonblocking -
																// (VK_PRESENT_MODE_FIFO_KHR)
																// blocks (vsync
																// on or off)

		createInfo.clipped = VK_TRUE;

		if(vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS)
		{
			ASSERT(false, "failed to create vkSwapchainKHR");
		}

		uint32 imageCount = 0;
		if(vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, nullptr) != VK_SUCCESS)
		{
			ASSERT(false, "failed to get imagecount");
		}

		m_Images = new VkImage[imageCount];
		if(vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, m_Images) != VK_SUCCESS)
		{
			ASSERT(false, "failed to get swapchainimages");
		}
		m_ImageViews = new VkImageView[imageCount];
		m_NofSwapchainImages = imageCount;
	}

	void VulkanDevice::CreateSwapchainSurface()
	{
		const Window* window = GraphicsEngine::Get().GetWindow();
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = static_cast<HWND>(window->GetHandle());
		createInfo.hinstance = ::GetModuleHandle(nullptr);

		if(vkCreateWin32SurfaceKHR(m_Instance, &createInfo, nullptr, &m_Surface) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to create Win32SurfaceKHR");
		}
	}

	void VulkanDevice::CreateImGuiContext(VkRenderPass renderPass, VkDescriptorPool descriptorPool,
										  VkCommandPool commandPool)
	{
		const Window::Size windowSize = GraphicsEngine::Get().GetWindow()->GetSize();

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize.x = windowSize.m_Width;
		io.DisplaySize.y = windowSize.m_Height;

		ImGui_ImplVulkan_InitInfo info = {};
		info.Device = m_Device;
		info.PhysicalDevice = m_PhysicalDevice;
		info.Instance = m_Instance;
		info.Queue = m_Queue;
		info.QueueFamily = m_QueueFamily;
		info.DescriptorPool = descriptorPool;
		info.MinImageCount = 2;
		info.ImageCount = m_NofSwapchainImages;

		if(!ImGui_ImplVulkan_Init(&info, renderPass))
			ASSERT(false, "Failed");

		if(vkResetCommandPool(m_Device, commandPool, 0) != VK_SUCCESS)
			ASSERT(false, "failed to reset commandPool");

		VkCommandBuffer command_buffer = BeginSingleTimeCommand(commandPool);

		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

		EndSingleTimeCommand(command_buffer, commandPool);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	void VulkanDevice::CreateDeviceSemaphore(VkSemaphore* semaphore)
	{
		VkSemaphoreCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		if(vkCreateSemaphore(m_Device, &create_info, nullptr, semaphore) != VK_SUCCESS)
			ASSERT(false, "failed to create semaphore!");
	}

	VkSurfaceFormatKHR VulkanDevice::GetPhysicalDeviceSurfaceFormat()
	{
		uint32 formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, nullptr);

		VkSurfaceFormatKHR* surfaceFormats = new VkSurfaceFormatKHR[formatCount];
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount,
											 surfaceFormats);

		VkSurfaceFormatKHR format = surfaceFormats[0];
		if(formatCount == 1 && format.format == VK_FORMAT_UNDEFINED)
		{
			format = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}
		else
		{
			for(int32 i = 0; i < formatCount; ++i)
			{
				VkSurfaceFormatKHR f = surfaceFormats[i];
				if(f.format == VK_FORMAT_B8G8R8A8_UNORM &&
				   f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					format = f;
			}
		}

		delete[] surfaceFormats;
		surfaceFormats = nullptr;

		return format;
	}

	VkDescriptorPool VulkanDevice::CreateDescriptorPool(VkDescriptorPoolSize* pool_sizes,
														int32 nof_pool_sizes, int32 max_sets)
	{

		VkDescriptorPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		create_info.poolSizeCount = nof_pool_sizes;
		create_info.maxSets = max_sets;
		create_info.pPoolSizes = pool_sizes;

		VkDescriptorPool pool = nullptr;
		if(vkCreateDescriptorPool(m_Device, &create_info, nullptr, &pool) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to create descriptor pool!");
		}

		return pool;
	}

	void VulkanDevice::CreateFence(VkFence* fence)
	{
		VkFenceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		if(vkCreateFence(m_Device, &create_info, nullptr, fence) != VK_SUCCESS)
			ASSERT(false, "failed to create VkFence!");
	}

	void VulkanDevice::UpdateDescriptorSets(uint32 nofSets, VkWriteDescriptorSet* writeSets,
											uint32 copyCount, VkCopyDescriptorSet* copySets)
	{
		vkUpdateDescriptorSets(m_Device, nofSets, writeSets, copyCount, copySets);
	}

	void VulkanDevice::SubmitQueue(const VkSubmitInfo& submitInfo, uint32 submitCount,
								   VkFence fence)
	{
		if(vkQueueSubmit(m_Queue, submitCount, &submitInfo, fence) != VK_SUCCESS)
			ASSERT(false, "Failed to submit queue!");
	}

	void VulkanDevice::PresentQueue(uint32 imageIndex, VkSemaphore* waitSemaphores,
									uint32 nofWaitSemaphore)
	{
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.swapchainCount = 1;
		info.pSwapchains = &m_Swapchain;
		info.pImageIndices = &imageIndex;
		info.pWaitSemaphores = waitSemaphores;
		info.waitSemaphoreCount = nofWaitSemaphore;

		if(vkQueuePresentKHR(m_Queue, &info) != VK_SUCCESS)
			ASSERT(false, "failed to present the queue!");
	}

	void VulkanDevice::AcquireNextImage(VkSemaphore acquireSema, VkFence acquireFence,
										uint32* imageIndex)
	{
		if(vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, acquireSema, acquireFence,
								 imageIndex) != VK_SUCCESS)
		{
			ASSERT(false, "failed to acquirenextimage!");
		}
	}

	void VulkanDevice::CreatePipelines(VkPipelineCache pipelinesCache, uint32 createInfoCount,
									   const VkGraphicsPipelineCreateInfo* createInfo,
									   VkPipeline* pipeline)
	{

		if(vkCreateGraphicsPipelines(m_Device, pipelinesCache, createInfoCount, createInfo, nullptr,
									 pipeline) != VK_SUCCESS)
		{
			ASSERT(false, "failed to create graphics pipelines!");
		}
	}

	VkPipelineShaderStageCreateInfo CreateShaderStageInfo(VkShaderStageFlagBits stageFlags,
														  VkShaderModule shaderModule,
														  const char* entrypoint)
	{
		VkPipelineShaderStageCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		create_info.stage = stageFlags;
		create_info.module = shaderModule;
		create_info.pName = entrypoint;
		return create_info;
	}

	void VulkanDevice::CreateDepthResources()
	{
		// get the depth format
		VkFormat depthFormat = {};
		VkFormat formats[] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
							   VK_FORMAT_D24_UNORM_S8_UINT };

		const VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
		for(VkFormat format : formats)
		{
			VkFormatProperties properties = {};
			vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &properties);

			if((properties.optimalTilingFeatures & features) == features)
				depthFormat = format;
		}

		const VkExtent2D extent = GetSurfaceCapabilities(m_Surface).currentExtent;

		auto imageData = Create2DImage(
			extent.width, extent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_DefaultDepthImage = std::get<0>(imageData);
		m_DepthMemory = std::get<1>(imageData); // not bound yet

		m_DefaultDepthImageView =
			Create2DImageView(depthFormat, m_DefaultDepthImage, VK_IMAGE_ASPECT_DEPTH_BIT);

		m_DepthFormat = depthFormat;

		// setup a commandpool and commandbuffer
		VkCommandPool pool = CreateCommandPool();
		VkCommandBuffer commandBuffer = BeginSingleTimeCommand(pool);

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

			if(HasStencilComponent(depthFormat))
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

		if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		   newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
				newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
				newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
									VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
		{
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0,
							 nullptr, 1, &barrier);

		EndSingleTimeCommand(commandBuffer, pool);

		DestroyObject(&vkDestroyCommandPool, pool, nullptr);
	}

	bool VulkanDevice::SurfaceCanPresent(VkSurfaceKHR surface)
	{
		VkBool32 can_present = VK_FALSE;
		if(vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, m_QueueFamily, surface,
												&can_present) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to query if surface can present!");
		}

		return (can_present == VK_TRUE);
	}

	VkSurfaceCapabilitiesKHR VulkanDevice::GetSurfaceCapabilities(VkSurfaceKHR surface) const
	{
		VkSurfaceCapabilitiesKHR capabilities = {};
		if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, surface, &capabilities) !=
		   VK_SUCCESS)
		{
			ASSERT(false, "Failed to get surface capabilities");
		}
		return capabilities;
	}

	VkCommandBuffer VulkanDevice::BeginSingleTimeCommand(VkCommandPool pool)
	{
		VkCommandBuffer buffer = {};
		CreateCommandBuffers(pool, &buffer, 1);

		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if(vkBeginCommandBuffer(buffer, &begin_info) != VK_SUCCESS)
			ASSERT(false, "failed to begin commandbuffer!");

		return buffer;
	}

	void VulkanDevice::EndSingleTimeCommand(VkCommandBuffer commandBuffer, VkCommandPool pool)
	{
		// end
		if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			ASSERT(false, "failed to end commandbuffer!");

		// submit data to graphics card
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		if(vkQueueSubmit(m_Queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
			ASSERT(false, "failed to submit vkQueue!");

		// wait
		if(vkQueueWaitIdle(m_Queue) != VK_SUCCESS)
			ASSERT(false, "vkQueueWaitIdle failed!");

		// cleanup when finished
		vkFreeCommandBuffers(m_Device, pool, 1, &commandBuffer);
	}

	bool VulkanDevice::HasStencilComponent(VkFormat format) const
	{
		return (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT);
	}

	void VulkanDevice::ResetFences(VkFence* fences, uint32 fenceCount)
	{
		if(vkResetFences(m_Device, fenceCount, fences) != VK_SUCCESS)
		{
			ASSERT(false, "failed to reset fences!");
		}
	}

	void VulkanDevice::WaitForFences(VkFence* fences, uint32 fenceCount, VkBool32 waitForAll)
	{
		if(vkWaitForFences(m_Device, fenceCount, fences, waitForAll, UINT64_MAX) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to wait for fences!");
		}
	}

}; // namespace Graphics
