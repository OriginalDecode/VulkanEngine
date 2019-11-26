#pragma once
#include <vulkan/vulkan_core.h>
namespace vlk
{
	enum EDepthStencilInfo
	{
		DEPTH_READ_WRITE,
	};

	/*
		 VkStructureType                           sType;
		 const void*                               pNext;
		 VkPipelineDepthStencilStateCreateFlags    flags;
		 VkBool32                                  depthTestEnable;
		 VkBool32                                  depthWriteEnable;
		 VkCompareOp                               depthCompareOp;
		 VkBool32                                  depthBoundsTestEnable;
		 VkBool32                                  stencilTestEnable;
		 VkStencilOpState                          front;
		 VkStencilOpState                          back;
		 float                                     minDepthBounds;
		 float                                     maxDepthBounds;

		 
		typedef struct VkStencilOpState {
			VkStencilOp    failOp;
			VkStencilOp    passOp;
			VkStencilOp    depthFailOp;
			VkCompareOp    compareOp;
			uint32_t       compareMask;
			uint32_t       writeMask;
			uint32_t       reference;
		} VkStencilOpState;
	*/

	const VkPipelineDepthStencilStateCreateInfo depthStencilInfos[] = {
		{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		  nullptr,
		  0,
		  VK_TRUE,
		  VK_TRUE,
		  VK_COMPARE_OP_LESS,
		  VK_FALSE,
		  VK_FALSE,
		  { VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS, 0xFF,
			0xFF, 0x0 },
		  { VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS, 0xFF,
			0xFF, 0x0 },
		  0.f,
		  1.f },

	};
}; // namespace vlk
