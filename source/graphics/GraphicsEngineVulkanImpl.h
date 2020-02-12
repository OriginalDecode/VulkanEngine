#pragma once
#include "core/Defines.h"

DEFINE_VK_HANDLE(VkSurfaceKHR);
DEFINE_VK_HANDLE(VkImage);
DEFINE_VK_HANDLE(VkImageView);
class Window;
namespace Graphics
{
	struct RenderContextVulkan;
	class GraphicsEngineVulkanImpl
	{
	public:
		GraphicsEngineVulkanImpl() = default;
		~GraphicsEngineVulkanImpl() = default;

		void Init(const Window& window);
		void Destroy();
		RenderContextVulkan* GetRenderContext() { return m_Context; }

	private:
		RenderContextVulkan* m_Context = nullptr;
		void CreatePhysicalDevice();
		void CreateLogicalDevice();
		void CreateSwapchain();
	};
}; // namespace Graphics
