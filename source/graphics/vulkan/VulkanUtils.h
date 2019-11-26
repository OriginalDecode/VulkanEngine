#pragma once
#include "core/Types.h"

#include <vulkan/vulkan_core.h>
#include <tuple>

namespace Graphics
{
	struct RenderContextVulkan;
};

namespace vlk
{

	VkShaderModule LoadShader(const char* filepath);

	void CreateInstance(const char* appName, int32 appVersion, const char* engineName,
						int32 engineVersion, int32 apiVersion, Graphics::RenderContextVulkan* ctx);

	void DestroyInstance(VkInstance instance);

	std::tuple<VkPhysicalDevice*, uint32> AllocPhysicalDeviceList(VkInstance instance);
	void FreePhysicalDeviceList(VkPhysicalDevice* list);

	std::tuple<VkQueueFamilyProperties*, uint32>
		AllocPhysicalDevicePropertiesList(VkPhysicalDevice device);
	void FreePhysicalDevicePropertiesList(VkQueueFamilyProperties* properties);

	std::tuple<VkSurfaceFormatKHR*, uint32>
		AllocPhysicalDeviceSurfaceFormatsList(VkSurfaceKHR surface);
	void FreePhysicalDeviceSurfaceFormatsList(VkSurfaceFormatKHR* formats);
	VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VkSurfaceKHR surface);
	VkSurfaceFormatKHR GetSurfaceFormat(VkSurfaceFormatKHR* formats, uint32 nof_formats,
										VkFormat format, VkColorSpaceKHR color_space);

	bool HasDepthStencilComponent(VkFormat format);

	VkCommandBuffer* CreateCommandBuffers(VkCommandPool pool, uint32 nof_buffers);

	VkCommandPool CreateCommandPool(VkFlags create_flags, uint32 queue_family);

	/*
		Takes an already made command buffer and reuses
	*/
	void BeginSingleTimeCommand(VkCommandBuffer buffer, VkCommandPool pool, VkQueue queue);

	/*
		You have to free the CommandBuffer yourself after you end the singletime command.
	*/
	void EndSingleTimeCommand(VkCommandBuffer buffer, VkCommandPool pool, VkQueue queue);

	void PresentQueue(VkQueue queue, uint32 imageIndex, VkSemaphore* waitSemaphores,
					  uint32 nofSemaphores, VkSwapchainKHR swapchain);

	VkPipelineShaderStageCreateInfo CreateShaderStageInfo(VkShaderStageFlagBits stageFlags,
														  VkShaderModule shaderModule,
														  const char* entrypoint);

	uint32 FindMemoryType(uint32 type_filter, VkMemoryPropertyFlags property_flags);

	bool CanPresent(VkSurfaceKHR surface);

	VkDeviceMemory AllocDeviceMemory(VkMemoryRequirements memory_req, uint32 memory_type_idx);

	VkDescriptorSet AllocDescSet(VkDescriptorPool desc_pool, uint32 nof_desc_set,
								 VkDescriptorSetLayout* layouts);

	VkFramebuffer CreateFramebuffer(VkImageView* image_views, uint32 nof_attachments, uint32 width,
									uint32 height, VkRenderPass render_pass);

	VkDescriptorPool CreateDescPool(VkDescriptorPoolSize* pool_sizes, uint32 size, uint32 max_sets);

	VkFence CreateFence();

	VkSemaphore CreateDeviceSemaphore();

	VkPipelineLayout CreatePipelineLayout(VkDescriptorSetLayout* layouts, uint32 nof_layouts,
										  VkPushConstantRange* pcr, uint32 nof_pcr);

	VkVertexInputAttributeDescription CreateAttributeDesc(VkFormat format, int32 binding,
														  int32 location, int32 offset);

	VkDescriptorSetLayout CreateDescLayout(VkDescriptorSetLayoutBinding* bindings,
										   uint32 nof_bindings);

	VkVertexInputBindingDescription CreateBindingDesc(uint32 binding, uint32 stride,
													  VkVertexInputRate input_rate);

	VkDescriptorSetLayoutBinding CreateLayoutBinding(uint32 binding, VkDescriptorType type,
													 VkShaderStageFlagBits shader_stage,
													 uint32 nof_descriptors);

	VkImageView Create2DImageView(VkFormat format, VkImage image, VkImageAspectFlags flags);

	std::tuple<VkImage, VkDeviceMemory> Create2DImage(uint32 width, uint32 height, VkFormat format,
													  VkImageTiling image_tilig,
													  VkImageUsageFlags usage_flags,
													  VkMemoryPropertyFlags memory_flags);

}; // namespace vlk
