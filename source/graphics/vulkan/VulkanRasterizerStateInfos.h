#pragma once
#include <vulkan/vulkan_core.h>
namespace vlk
{
	enum ERasterizerStateInfo
	{
		FILL_CULL_BACK_CLOCKWISE,
		WIRE_CULL_BACK_CLOCKWISE,
	};

	/*
		VkStructureType                            sType;
		const void*                                pNext;
		VkPipelineRasterizationStateCreateFlags    flags;
		VkBool32                                   depthClampEnable;
		VkBool32                                   rasterizerDiscardEnable;
		VkPolygonMode                              polygonMode;
		VkCullModeFlags                            cullMode;
		VkFrontFace                                frontFace;
		VkBool32                                   depthBiasEnable;
		float                                      depthBiasConstantFactor;
		float                                      depthBiasClamp;
		float                                      depthBiasSlopeFactor;
		float                                      lineWidth;
	*/

	const VkPipelineRasterizationStateCreateInfo rasterizerInfos[] = {
		{
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_FALSE,
			VK_FALSE,
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_CLOCKWISE,
			VK_FALSE,
			0.f,
			0.f,
			0.f,
			1.f,
		},
        {
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_FALSE,
			VK_FALSE,
			VK_POLYGON_MODE_LINE,
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_CLOCKWISE,
			VK_FALSE,
			0.f,
			0.f,
			0.f,
			1.f,
		},
	};
}; // namespace vlk