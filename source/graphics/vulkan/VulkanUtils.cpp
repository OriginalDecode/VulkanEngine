#include "VulkanUtils.h"
#include <vulkan/vulkan.h>

#include "Core/Types.h"
#include "logger/Debug.h"
#include "graphics/RenderContextVulkan.h"
#include "Core/File.h"

#ifndef ARRSIZE
#define ARRSIZE(x) sizeof(x) / sizeof(x[0])
#endif
#define VK_GET_FNC_POINTER(function, instance) (PFN_##function) vkGetInstanceProcAddr(instance, #function)
namespace vlk
{
	VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT /* flags */, VkDebugReportObjectTypeEXT /* objectType */, uint64_t /* object */, size_t /* location */,
													   int32_t /* messageCode */, const char* /* pLayerPrefix */, const char* pMessage, void* /* pUserData */)
	{
		char temp[USHRT_MAX] = { 0 };
		sprintf_s(temp, "Vulkan Warning :%s", pMessage);
		OutputDebugStringA(pMessage);
		OutputDebugStringA("\n");
		LOG_MESSAGE(temp);
		// assert( false && temp );
		return VK_FALSE;
	}
	VkDebugReportCallbackEXT debugCallback = nullptr;
	Graphics::RenderContextVulkan* g_Context = nullptr;

	VkShaderModule LoadShader(const char* filepath)
	{
		Core::File shader(filepath, Core::FileMode::READ_FILE);
		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = shader.GetSize();
		create_info.pCode = (const uint32_t*)shader.GetBuffer();

		VkShaderModule shaderModule = nullptr;
		VkResult result = vkCreateShaderModule(g_Context->Device, &create_info, nullptr, &shaderModule);
		ASSERT(result == VK_SUCCESS, "Failed to create VkShaderModule!");

		return shaderModule;
	}

	void DestroyShader(VkShaderModule module) { vkDestroyShaderModule(g_Context->Device, module, nullptr); }

	void CreateInstance(const char* appName, int32 appVersion, const char* engineName, int32 engineVersion, int32 apiVersion, Graphics::RenderContextVulkan* ctx)
	{
		g_Context = ctx; // set the "global" context

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = appName;
		appInfo.applicationVersion = appVersion;
		appInfo.pEngineName = engineName;
		appInfo.engineVersion = engineVersion; // VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = apiVersion;	   // VK_API_VERSION_1_1;

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;

		const char* validationLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
		instanceCreateInfo.enabledLayerCount = ARRSIZE(validationLayers);
		instanceCreateInfo.ppEnabledLayerNames = &validationLayers[0];

		const char* extentions[] = { "VK_KHR_surface", "VK_KHR_win32_surface", "VK_EXT_debug_report" };
		instanceCreateInfo.enabledExtensionCount = ARRSIZE(extentions);
		instanceCreateInfo.ppEnabledExtensionNames = extentions;
		VkInstance instance;
		VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr /*allocator*/, &instance);
		ASSERT(result == VK_SUCCESS, "Failed to create Vulkan instance!");

		auto create = VK_GET_FNC_POINTER(vkCreateDebugReportCallbackEXT, instance);
		ASSERT(create, "Failed to create callback function!");

		VkDebugReportFlagsEXT flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;

		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.pfnCallback = &DebugReportCallback;
		createInfo.flags = flags;

		result = create(instance, &createInfo, nullptr, &debugCallback);
		ASSERT(result == VK_SUCCESS, "Failed to create vulkan debug callback!");

		g_Context->Instance = instance;
	}

	void DestroyInstance(VkInstance instance)
	{
		auto destoryer = VK_GET_FNC_POINTER(vkDestroyDebugReportCallbackEXT, instance);
		destoryer(instance, debugCallback, nullptr);
		vkDestroyInstance(instance, nullptr);
	}

	std::tuple<VkPhysicalDevice*, uint32> AllocPhysicalDeviceList(VkInstance instance)
	{
		uint32 nof_devices;
		VkResult result = vkEnumeratePhysicalDevices(instance, &nof_devices, nullptr);
		ASSERT(result == VK_SUCCESS, "Failed to enumerate gpus!");

		VkPhysicalDevice* device_list = new VkPhysicalDevice[nof_devices];
		result = vkEnumeratePhysicalDevices(instance, &nof_devices, device_list);
		ASSERT(result == VK_SUCCESS, "Failed to enumerate device!");

		return { device_list, nof_devices };
	}

	void AllocPhysicalDeviceList(VkInstance instance, VkPhysicalDevice* deviceList, uint32* nofDevices)
	{
		VkResult result = vkEnumeratePhysicalDevices(instance, nofDevices, nullptr);
		ASSERT(result == VK_SUCCESS, "Failed to enumerate gpus!");

		deviceList = new VkPhysicalDevice[*nofDevices];
		result = vkEnumeratePhysicalDevices(instance, nofDevices, deviceList);
		ASSERT(result == VK_SUCCESS, "Failed to enumerate device!");
	}

	void FreeList(void* list)
	{
		delete[] list;
		list = nullptr;
	}

	void FreePhysicalDeviceList(VkPhysicalDevice* list)
	{
		delete[] list;
		list = nullptr;
	}

	void FreePhysicalDevicePropertiesList(VkQueueFamilyProperties* properties)
	{
		delete[] properties;
		properties = nullptr;
	}

	void FreePhysicalDeviceSurfaceFormatsList(VkSurfaceFormatKHR* formats)
	{
		delete[] formats;
		formats = nullptr;
	}

	std::tuple<VkQueueFamilyProperties*, uint32> AllocPhysicalDevicePropertiesList(VkPhysicalDevice device)
	{
		uint32 nof_properties;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &nof_properties, nullptr);
		VkQueueFamilyProperties* properties = new VkQueueFamilyProperties[nof_properties];
		vkGetPhysicalDeviceQueueFamilyProperties(device, &nof_properties, properties);
		return { properties, nof_properties };
	}

	std::tuple<VkSurfaceFormatKHR*, uint32> AllocPhysicalDeviceSurfaceFormatsList(VkSurfaceKHR surface)
	{
		uint32 nof_formats;
		if(vkGetPhysicalDeviceSurfaceFormatsKHR(g_Context->PhysicalDevice, surface, &nof_formats, nullptr) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to get formats from surface!");
			return { nullptr, 0 };
		}
		VkSurfaceFormatKHR* formats = new VkSurfaceFormatKHR[nof_formats];
		if(vkGetPhysicalDeviceSurfaceFormatsKHR(g_Context->PhysicalDevice, surface, &nof_formats, formats) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to allocate formats list!");
			delete[] formats;
			formats = nullptr;
			nof_formats = 0;
		}

		return { formats, nof_formats };
	}

	VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VkSurfaceKHR surface)
	{
		VkSurfaceCapabilitiesKHR capabilities = {};
		if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_Context->PhysicalDevice, surface, &capabilities) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to get surface capabilities");
		}
		return capabilities;
	}

	VkSurfaceFormatKHR GetSurfaceFormat(VkSurfaceFormatKHR* formats, uint32 nof_formats, VkFormat format, VkColorSpaceKHR color_space)
	{
		VkSurfaceFormatKHR format_usage = {};
		if(nof_formats == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
		{
			format_usage.format = format;
			format_usage.colorSpace = color_space;
		}
		else
		{
			for(uint32 i = 0; i < nof_formats; ++i)
			{
				VkSurfaceFormatKHR f = formats[i];
				if(f.format == format && f.colorSpace == color_space)
				{
					format_usage.format = f.format;
					format_usage.colorSpace = f.colorSpace;
					break;
				}
			}
		}

		return format_usage;
	}

	bool HasDepthStencilComponent(VkFormat format) { return (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT); }

	void BeginSingleTimeCommand(VkCommandBuffer buffer)
	{
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VkResult result = vkBeginCommandBuffer(buffer, &begin_info);
		ASSERT(result == VK_SUCCESS, "failed to begin commandbuffer!");
	}

	void EndSingleTimeCommand(VkCommandBuffer buffer, VkQueue queue)
	{
		VkResult result = vkEndCommandBuffer(buffer);
		ASSERT(result == VK_SUCCESS, "Failed to end commandbuffer!");

		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &buffer;

		result = vkQueueSubmit(queue, 1, &info, VK_NULL_HANDLE);
		ASSERT(result == VK_SUCCESS, "vkQueueWaitIdle failed!");

		result = vkDeviceWaitIdle(g_Context->Device);
		ASSERT(result == VK_SUCCESS, "Device failed to wait");
	}

	VkCommandBuffer* CreateCommandBuffers(VkCommandPool pool, uint32 nof_buffers)
	{
		VkCommandBuffer* buffers = new VkCommandBuffer[nof_buffers];
		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = pool;
		alloc_info.commandBufferCount = nof_buffers;

		VkResult result = vkAllocateCommandBuffers(g_Context->Device, &alloc_info, buffers);
		ASSERT(result == VK_SUCCESS, "Failed to create VkCommandBuffers!");

		return buffers;
	}

	VkCommandPool CreateCommandPool(VkFlags create_flags, uint32 queue_family)
	{
		VkCommandPool command_pool = {};
		VkCommandPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = create_flags;
		create_info.queueFamilyIndex = queue_family;

		VkResult result = vkCreateCommandPool(g_Context->Device, &create_info, nullptr, &command_pool);
		ASSERT(result == VK_SUCCESS, "Failed to create commandpool");
		return command_pool;
	}

	void PresentQueue(VkQueue queue, uint32 imageIndex, VkSemaphore* waitSemaphores, uint32 nofSemaphores, VkSwapchainKHR swapchain)
	{
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.swapchainCount = 1; /* unsure about this one, might change in the future */
		info.pSwapchains = &swapchain;
		info.pImageIndices = &imageIndex;
		info.pWaitSemaphores = waitSemaphores;
		info.waitSemaphoreCount = nofSemaphores;

		VkResult result = vkQueuePresentKHR(queue, &info);
		ASSERT(result == VK_SUCCESS, "Failed to present the queue!");
	}

	VkPipelineShaderStageCreateInfo CreateShaderStageInfo(VkShaderStageFlagBits stageFlags, VkShaderModule shaderModule, const char* entrypoint)
	{
		VkPipelineShaderStageCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		create_info.stage = stageFlags;
		create_info.module = shaderModule;
		create_info.pName = entrypoint;
		return create_info;
	}

	uint32 FindMemoryType(uint32 type_filter, VkMemoryPropertyFlags property_flags)
	{
		VkPhysicalDeviceMemoryProperties memory_properties = {};
		vkGetPhysicalDeviceMemoryProperties(g_Context->PhysicalDevice, &memory_properties);
		for(uint32 i = 0; i < memory_properties.memoryTypeCount; ++i)
		{
			if((type_filter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags)
			{
				return i;
			}
		}
		ASSERT(false, "failed to find suitable memory type!");
		return 0;
	}

	bool CanPresent(VkSurfaceKHR surface)
	{
		VkBool32 can_present = VK_FALSE;
		if(vkGetPhysicalDeviceSurfaceSupportKHR(g_Context->PhysicalDevice, g_Context->QueueFamily, surface, &can_present) != VK_SUCCESS)
		{
			ASSERT(false, "Failed to query if surface can present");
			return false;
		}

		return (can_present == VK_TRUE);
	}

	VkDeviceMemory AllocDeviceMemory(VkMemoryRequirements memory_req, uint32 memory_type_idx)
	{
		VkDeviceMemory memory = nullptr;
		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = memory_req.size;
		alloc_info.pNext = nullptr;
		alloc_info.memoryTypeIndex = memory_type_idx;

		VkResult result = vkAllocateMemory(g_Context->Device, &alloc_info, nullptr, &memory);
		ASSERT(result == VK_SUCCESS, "Failed to allocate memory on GPU");

		return memory;
	}

	VkDescriptorSet AllocDescSet(VkDescriptorPool desc_pool, uint32 nof_desc_set, VkDescriptorSetLayout* layouts)
	{
		VkDescriptorSetAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.descriptorSetCount = nof_desc_set;
		alloc_info.pSetLayouts = layouts;
		alloc_info.descriptorPool = desc_pool;
		VkDescriptorSet set = nullptr;
		VkResult result = vkAllocateDescriptorSets(g_Context->Device, &alloc_info, &set);
		ASSERT(result == VK_SUCCESS, "Failed to allocate DescriptorSet!");

		return set;
	}

	VkFramebuffer CreateFramebuffer(VkImageView* image_views, uint32 nof_attachments, uint32 width, uint32 height, VkRenderPass render_pass)
	{
		VkFramebufferCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		create_info.renderPass = render_pass;
		create_info.attachmentCount = nof_attachments;
		create_info.pAttachments = image_views;
		create_info.width = width;
		create_info.height = height;
		create_info.layers = 1;

		VkFramebuffer framebuffer = nullptr;
		VkResult result = vkCreateFramebuffer(g_Context->Device, &create_info, nullptr, &framebuffer);
		ASSERT(result == VK_SUCCESS, "Failed to create framebuffer!");

		return framebuffer;
	}

	VkDescriptorPool CreateDescPool(VkDescriptorPoolSize* pool_sizes, uint32 size, uint32 max_sets)
	{
		VkDescriptorPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		create_info.poolSizeCount = size;
		create_info.maxSets = max_sets;
		create_info.pPoolSizes = pool_sizes;

		VkDescriptorPool pool = nullptr;
		VkResult result = vkCreateDescriptorPool(g_Context->Device, &create_info, nullptr, &pool);
		ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool!");

		return pool;
	}

	VkFence CreateFence()
	{
		VkFenceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		VkFence fence;
		VkResult result = vkCreateFence(g_Context->Device, &create_info, nullptr, &fence);
		ASSERT(result == VK_SUCCESS, "failed to create VkFence!");

		return fence;
	}

	VkSemaphore CreateDeviceSemaphore()
	{
		VkSemaphoreCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VkSemaphore semaphore;
		VkResult result = vkCreateSemaphore(g_Context->Device, &create_info, nullptr, &semaphore);
		ASSERT(result == VK_SUCCESS, "failed to create semaphore!");

		return semaphore;
	}

	VkPipelineLayout CreatePipelineLayout(VkDescriptorSetLayout* layouts, uint32 nof_layouts, const VkPushConstantRange* pcr, uint32 nof_pcr)
	{
		VkPipelineLayoutCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		pipelineCreateInfo.setLayoutCount = nof_layouts;
		pipelineCreateInfo.pSetLayouts = layouts;

		pipelineCreateInfo.pushConstantRangeCount = nof_pcr;
		pipelineCreateInfo.pPushConstantRanges = pcr;

		VkPipelineLayout pipeline;
		VkResult result = vkCreatePipelineLayout(g_Context->Device, &pipelineCreateInfo, nullptr, &pipeline);
		ASSERT(result == VK_SUCCESS, "Failed to create pipelinelayout!");

		return pipeline;
	}

	VkVertexInputAttributeDescription CreateAttributeDesc(VkFormat format, int32 binding, int32 location, int32 offset)
	{
		VkVertexInputAttributeDescription desc = {};
		desc.binding = binding;
		desc.location = location;
		desc.format = format;
		desc.offset = offset;
		return desc;
	}

	VkDescriptorSetLayout CreateDescLayout(const VkDescriptorSetLayoutBinding* bindings, uint32 nof_bindings)
	{
		VkDescriptorSetLayoutCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		create_info.bindingCount = nof_bindings;
		create_info.pBindings = bindings;
		VkDescriptorSetLayout layout = nullptr;
		VkResult result = vkCreateDescriptorSetLayout(g_Context->Device, &create_info, nullptr, &layout);
		ASSERT(result == VK_SUCCESS, "Failed to create DescriptorSetLayout");

		return layout;
	}

	VkVertexInputBindingDescription CreateBindingDesc(uint32 binding, uint32 stride, VkVertexInputRate input_rate)
	{
		VkVertexInputBindingDescription desc = {};
		desc.binding = binding;
		desc.stride = stride;
		desc.inputRate = input_rate;
		return desc;
	}

	VkDescriptorSetLayoutBinding CreateLayoutBinding(uint32 binding, VkDescriptorType type, VkShaderStageFlagBits shader_stage, uint32 nof_descriptors)
	{
		VkDescriptorSetLayoutBinding layout_binding = {};
		layout_binding.binding = binding;
		layout_binding.descriptorType = type;
		layout_binding.stageFlags = shader_stage;
		layout_binding.descriptorCount = nof_descriptors;
		return layout_binding;
	}

	VkImageView Create2DImageView(VkFormat format, VkImage image, VkImageAspectFlags flags)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = image;
		create_info.format = format;
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
		VkImageSubresourceRange& srr = create_info.subresourceRange;
		srr.aspectMask = flags;
		srr.baseMipLevel = 0;
		srr.levelCount = 1;
		srr.baseArrayLayer = 0; // related to 3d type
		srr.layerCount = 1;		// related to 3d type

		VkImageView view = nullptr;
		VkResult result = vkCreateImageView(g_Context->Device, &create_info, nullptr, &view);
		ASSERT(result == VK_SUCCESS, "Failed to create ImageView!");

		return view;
	}

	const char* GetFailReason(VkResult result)
	{
		switch(result)
		{
			case VK_SUCCESS:
				return "VK_SUCCESS";
			case VK_NOT_READY:
				return "VK_NOT_READY";
			case VK_TIMEOUT:
				return "VK_TIMEOUT";
			case VK_EVENT_SET:
				return "VK_EVENT_SET";
			case VK_EVENT_RESET:
				return "VK_EVENT_RESET";
			case VK_INCOMPLETE:
				return "VK_INCOMPLETE";
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				return "VK_ERROR_OUT_OF_HOST_MEMORY";
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
			case VK_ERROR_INITIALIZATION_FAILED:
				return "VK_ERROR_INITIALIZATION_FAILED";
			case VK_ERROR_DEVICE_LOST:
				return "VK_ERROR_DEVICE_LOST";
			case VK_ERROR_MEMORY_MAP_FAILED:
				return "VK_ERROR_MEMORY_MAP_FAILED";
			case VK_ERROR_LAYER_NOT_PRESENT:
				return "VK_ERROR_LAYER_NOT_PRESENT";
			case VK_ERROR_EXTENSION_NOT_PRESENT:
				return "VK_ERROR_EXTENSION_NOT_PRESENT";
			case VK_ERROR_FEATURE_NOT_PRESENT:
				return "VK_ERROR_FEATURE_NOT_PRESENT";
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				return "VK_ERROR_INCOMPATIBLE_DRIVER";
			case VK_ERROR_TOO_MANY_OBJECTS:
				return "VK_ERROR_TOO_MANY_OBJECTS";
			case VK_ERROR_FORMAT_NOT_SUPPORTED:
				return "VK_ERROR_FORMAT_NOT_SUPPORTED";
			case VK_ERROR_FRAGMENTED_POOL:
				return "VK_ERROR_FRAGMENTED_POOL";
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				return "VK_ERROR_OUT_OF_POOL_MEMORY";
			case VK_ERROR_INVALID_EXTERNAL_HANDLE:
				return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
			case VK_ERROR_SURFACE_LOST_KHR:
				return "VK_ERROR_SURFACE_LOST_KHR";
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
				return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
			case VK_SUBOPTIMAL_KHR:
				return "VK_SUBOPTIMAL_KHR";
			case VK_ERROR_OUT_OF_DATE_KHR:
				return "VK_ERROR_OUT_OF_DATE_KHR";
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
				return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
			case VK_ERROR_VALIDATION_FAILED_EXT:
				return "VK_ERROR_VALIDATION_FAILED_EXT";
			case VK_ERROR_INVALID_SHADER_NV:
				return "VK_ERROR_INVALID_SHADER_NV";
			case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
				return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
			case VK_ERROR_FRAGMENTATION_EXT:
				return "VK_ERROR_FRAGMENTATION_EXT";
			case VK_ERROR_NOT_PERMITTED_EXT:
				return "VK_ERROR_NOT_PERMITTED_EXT";
			case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
				return "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT";
			case VK_RESULT_RANGE_SIZE:
				return "VK_RESULT_RANGE_SIZE";
		}
	}

	std::tuple<VkImage, VkDeviceMemory> Create2DImage(const ImageCreateInfo& image_create_info)
	{
		VkImageCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		create_info.imageType = VK_IMAGE_TYPE_2D;
		create_info.extent.width = image_create_info.Width;
		create_info.extent.height = image_create_info.Height;
		create_info.extent.depth = 1;
		create_info.mipLevels = 1;
		create_info.arrayLayers = 1;
		create_info.format = image_create_info.ImageFormat;
		create_info.tiling = image_create_info.ImageTiling;
		create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		create_info.usage = image_create_info.UsageFlags;
		create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkImage image = nullptr;
		VkResult result = vkCreateImage(g_Context->Device, &create_info, nullptr, &image);
		ASSERT(result == VK_SUCCESS, "failed to create image!");

		VkMemoryRequirements memory_requirements = {};
		vkGetImageMemoryRequirements(g_Context->Device, image, &memory_requirements);

		VkDeviceMemory memory = AllocDeviceMemory(memory_requirements, FindMemoryType(memory_requirements.memoryTypeBits, image_create_info.MemoryFlags));

		result = vkBindImageMemory(g_Context->Device, image, memory, image_create_info.Offset);
		ASSERT(result == VK_SUCCESS, "Failed to bind image memory!");

		return { image, memory };
	}

}; // namespace vlk
