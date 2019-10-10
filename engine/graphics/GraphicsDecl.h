#pragma once
#include <memory>
namespace Graphics
{
	using upVlkDevice = std::unique_ptr<class VlkDevice>;
	using upVlkSurface = std::unique_ptr<class VlkSurface>;
	using upVlkInstance = std::unique_ptr<class VlkInstance>;
	using upVlkSwapchain = std::unique_ptr<class VlkSwapchain>;
	using upVlkPhysicalDevice = std::unique_ptr<class VlkPhysicalDevice>;
}; // namespace Graphics
