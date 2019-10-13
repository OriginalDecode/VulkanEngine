#pragma once
#include "core/Defines.h"
#include "core/Types.h"
#include "graphics/IGfxDevice.h"

#include <vulkan/vulkan_core.h>
LPE_DEFINE_HANDLE(VkDevice);
LPE_DEFINE_HANDLE(VkQueue);
LPE_DEFINE_HANDLE(VkSwapchainKHR);
LPE_DEFINE_HANDLE(VkSurfaceKHR);

#include <tuple>

struct VkSwapchainCreateInfoKHR;
struct VkWin32SurfaceCreateInfoKHR;

namespace Graphics
{
	VkPipelineShaderStageCreateInfo CreateShaderStageInfo(VkShaderStageFlagBits stageFlags,
														  VkShaderModule shaderModule,
														  const char* entrypoint);
	class VulkanDevice
	{
	public:
		VulkanDevice() = default;
		~VulkanDevice();

		void Init();

		void CreateBuffer(const VkBufferCreateInfo& create_info, VkBuffer* buffer);
		VkDeviceMemory AllocMemory(VkMemoryRequirements memory_requirements,
								   uint32 memory_type_index);

		VkDescriptorSet AllocDescriptorSet(VkDescriptorPool descPool, uint32 descCount,
										   VkDescriptorSetLayout* layouts);

		void BindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, int32 offset);
		void BindImageMemory(VkImage buffer, VkDeviceMemory memory, int32 offset);

		uint32 FindMemoryType(uint32 type_filter, VkMemoryPropertyFlags flags);

		template <typename F, typename T, typename P>
		void DestroyObject(F fnc, T object, P other);

		void FreeCommandBuffers(VkCommandPool command_pool, VkCommandBuffer* buffers,
								uint32 nof_buffers);

		std::tuple<VkImage, VkDeviceMemory>
			Create2DImage(uint32 width, uint32 height, VkFormat format, VkImageTiling image_tilig,
						  VkImageUsageFlags usage_flags, VkMemoryPropertyFlags memory_flags);

		VkImageView Create2DImageView(VkFormat format, VkImage image, VkImageAspectFlags flags);

		VkFramebuffer CreateFramebuffer(VkImageView* image_views, uint32 nof_attachments,
										uint32 width, uint32 height, VkRenderPass renderpass);

		void CreateCommandBuffers(VkCommandPool command_pool, VkCommandBuffer* buffers,
								  uint32 nof_buffers);

		VkCommandPool CreateCommandPool();

		uint32 GetSwapchainImageCount() const { return m_NofSwapchainImages; }

		void CreateImGuiContext(VkRenderPass renderPass, VkDescriptorPool descriptorPool,
								VkCommandPool commandPool);

		void CreateDeviceSemaphore(VkSemaphore* semaphore);

		VkImage* GetImageList() { return m_Images; }
		VkImageView* GetViewList() { return m_ImageViews; }

		VkImageView GetDepthView() { return m_DefaultDepthImageView; }
		VkImage GetDepthImage() { return m_DefaultDepthImage; }

		VkSurfaceFormatKHR GetPhysicalDeviceSurfaceFormat();

		VkDescriptorPool CreateDescriptorPool(VkDescriptorPoolSize* pool_sizes,
											  int32 nof_pool_sizes, int32 max_sets);
		void CreateFence(VkFence* fence);

		void WaitForFences(VkFence* fences, uint32 fenceCount, VkBool32 waitForAll);
		void ResetFences(VkFence* fences, uint32 fenceCount);

		template <typename F, typename T, typename O>
		void CreateObject(F fnc, T createInfo, O object);

		VkFormat GetDepthFormat() const { return m_DepthFormat; }

		void UpdateDescriptorSets(uint32 nofSets, VkWriteDescriptorSet* writeSets, uint32 copyCount,
								  VkCopyDescriptorSet* copySets);

		void SubmitQueue(const VkSubmitInfo& submitInfo, uint32 submitCount, VkFence fence);

		void PresentQueue(uint32 imageIndex, VkSemaphore* waitSemaphores, uint32 nofWaitSemaphore);

		void AcquireNextImage(VkSemaphore acquireSema, VkFence acquireFence, uint32* imageIndex);

		void CreatePipelines(VkPipelineCache pipelinesCache, uint32 createInfoCount,
							 const VkGraphicsPipelineCreateInfo* createInfo, VkPipeline* pipeline);

	private:
		void CreateInstance();
		void CreatePhysicalDevice();
		void CreateDevice();
		void CreateSwapchain();
		void CreateSwapchainSurface();

		void CreateDepthResources();

		bool SurfaceCanPresent(VkSurfaceKHR surface);
		VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VkSurfaceKHR surface) const;

		VkCommandBuffer BeginSingleTimeCommand(VkCommandPool pool);
		void EndSingleTimeCommand(VkCommandBuffer commandBuffer, VkCommandPool pool);
		bool HasStencilComponent(VkFormat format) const;

		VkInstance m_Instance{ nullptr };
		VkPhysicalDevice m_PhysicalDevice{ nullptr };
		VkDevice m_Device{ nullptr };
		VkQueue m_Queue{ nullptr };
		VkSwapchainKHR m_Swapchain{ nullptr };
		VkSurfaceKHR m_Surface{ nullptr };

		uint32 m_NofSwapchainImages{ 0 };
		VkImage* m_Images{ nullptr };
		VkImageView* m_ImageViews{ nullptr };

		VkImage m_DefaultDepthImage{ nullptr };
		VkImageView m_DefaultDepthImageView{ nullptr };
		VkDeviceMemory m_DepthMemory{ nullptr };
		VkFormat m_DepthFormat;

		uint32 m_QueueFamily{ 0 };
		uint32 m_PresentFamily{ 0 };

		// std::vector<VkQueueFamilyProperties> m_QueueProperties;
	};

	template <typename F, typename T, typename P>
	void VulkanDevice::DestroyObject(F fnc, T object, P other)
	{
		fnc(m_Device, object, other);
	}

	template <typename F, typename T, typename O>
	void VulkanDevice::CreateObject(F fnc, T createInfo, O object)
	{
		if(fnc(m_Device, createInfo, nullptr, object) != VK_SUCCESS)
			ASSERT(false, "Failed to create vulkan object!");
	}

}; // namespace Graphics
