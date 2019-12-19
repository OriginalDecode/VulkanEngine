#pragma once
#include "Core/Defines.h"
#include "Core/Types.h"

DEFINE_VK_HANDLE(VkImageView);
DEFINE_VK_HANDLE(VkImage);

class VulkanFrameBuffer
{
public:
	VulkanFrameBuffer();
	~VulkanFrameBuffer();

private:
	VkImageView* m_ImageViews = nullptr;
	int32 m_NofViews{};
};
