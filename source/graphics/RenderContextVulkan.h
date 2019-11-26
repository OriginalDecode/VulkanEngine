#pragma once
#include "core/Types.h"
#include "graphics/vulkan/VulkanUtils.h"

class Window;
namespace Graphics
{
	struct SwapchainContext
	{
		VkSwapchainKHR Swapchain = nullptr;
		VkImage* Images = nullptr;
		VkImageView* Views = nullptr;
		uint32 ImageCount{};
	};

	struct RenderContextVulkan
	{
		const Window* Window = nullptr;
		VkInstance Instance = nullptr;
		VkDevice Device = nullptr;
		VkPhysicalDevice PhysicalDevice = nullptr;
		VkQueue Queue = nullptr;
		VkSurfaceKHR Surface = nullptr;
		SwapchainContext SwapchainCtx;

		uint32 QueueFamily{};
		uint32 PresentFamily{};
	};

}; // namespace Graphics
