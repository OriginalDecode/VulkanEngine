#include "VulkanGraphicsPipeline.h"

void VulkanGraphicsPipeline::Init(const VulkanGraphicsPipelineParams& pipeline_params)
{
	VkPipelineViewportStateCreateInfo viewportStateInfo = {};
	SetupViewportStateInfo(&viewportStateInfo, pipeline_params);

	VkPipelineColorBlendStateCreateInfo blendStateInfo = {};
	SetupBlendStateInfo(&blendStateInfo, pipeline_params);

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {};
	SetupInputAssemblerInfo(&inputAssemblyStateInfo, pipeline_params);

	VkPipelineVertexInputStateCreateInfo vtxInputStateInfo = {};
	SetupVertexInputStateInfo(&vtxInputStateInfo, pipeline_params);

	VkPipelineRasterizationStateCreateInfo rasterizerStateInfo = {};
	SetupRasetrizerInfo(&rasterizerStateInfo, pipeline_params);

	VkPipelineMultisampleStateCreateInfo multiSampleStateInfo = {};
	SetupMultiSamplerInfo(&multiSampleStateInfo, pipeline_params);

	VkPipelineLayoutCreateInfo layoutInfo = {};
	SetupPipelineLayout(&layoutInfo, pipeline_params);

	VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {};
	SetupDepthStencilInfo(&depthStencilStateInfo, pipeline_params);

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
	SetupGraphicsPipelineInfo(&graphicsPipelineCreateInfo, layoutInfo, rasterizerStateInfo, vtxInputStateInfo, inputAssemblyStateInfo, blendStateInfo, multiSampleStateInfo,
							  viewportStateInfo, depthStencilStateInfo, pipeline_params);

	vkCreateGraphicsPipelines(nullptr /*device*/, nullptr /*pipelineCache*/, 1, &graphicsPipelineCreateInfo, nullptr /*allocator*/, nullptr /*pipelines*/);
}

void VulkanGraphicsPipeline::Release() {}

void VulkanGraphicsPipeline::SetupViewportStateInfo(VkPipelineViewportStateCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params) {}

void VulkanGraphicsPipeline::SetupMultiSamplerInfo(VkPipelineMultisampleStateCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params) {}

void VulkanGraphicsPipeline::SetupBlendStateInfo(VkPipelineColorBlendStateCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params)
{
	VkPipelineColorBlendAttachmentState blendAttachState = {};
	blendAttachState.colorWriteMask = pipeline_params.ColorWriteMask;
	blendAttachState.blendEnable = pipeline_params.BlendEnable;

	info->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	info->logicOpEnable = VK_FALSE;
	info->attachmentCount = 1;
	info->pAttachments = &blendAttachState;
}

void VulkanGraphicsPipeline::SetupInputAssemblerInfo(VkPipelineInputAssemblyStateCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params)
{
	info->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	info->topology = pipeline_params.Topology;
	info->primitiveRestartEnable = VK_FALSE;
}

void VulkanGraphicsPipeline::SetupVertexInputStateInfo(VkPipelineVertexInputStateCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params)
{
	info->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	info->pVertexBindingDescriptions = &pipeline_params.Bindings[0];
	info->vertexBindingDescriptionCount = pipeline_params.Bindings.Size();

	info->pVertexAttributeDescriptions = &pipeline_params.Attributes[0];
	info->vertexAttributeDescriptionCount = pipeline_params.Attributes.Size();
}

void VulkanGraphicsPipeline::SetupRasetrizerInfo(VkPipelineRasterizationStateCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params) {}

void VulkanGraphicsPipeline::SetupDepthStencilInfo(VkPipelineDepthStencilStateCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params) {}

void VulkanGraphicsPipeline::SetupPipelineLayout(VkPipelineLayoutCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params) {}

void VulkanGraphicsPipeline::SetupGraphicsPipelineInfo(VkGraphicsPipelineCreateInfo* info, const VkPipelineLayoutCreateInfo& layout_info,
													   const VkPipelineRasterizationStateCreateInfo& raster_info, const VkPipelineVertexInputStateCreateInfo& vtx_input_info,
													   const VkPipelineInputAssemblyStateCreateInfo& ia_info, const VkPipelineColorBlendStateCreateInfo& blend_info,
													   const VkPipelineMultisampleStateCreateInfo& multi_sample_info, const VkPipelineViewportStateCreateInfo& vp_info,
													   const VkPipelineDepthStencilStateCreateInfo& depth_state_info, const VulkanGraphicsPipelineParams& params)
{

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.layout = nullptr;					  /* pipeline layout */
	pipelineInfo.pVertexInputState = &vtx_input_info; /* */
	pipelineInfo.renderPass = params.RenderPass;
	pipelineInfo.pViewportState = &vp_info;

	pipelineInfo.pInputAssemblyState = &ia_info; /* */
	pipelineInfo.pColorBlendState = &blend_info;
	pipelineInfo.pRasterizationState = &raster_info;
	pipelineInfo.pDepthStencilState = &depth_state_info;
	pipelineInfo.pMultisampleState = &multi_sample_info;

	pipelineInfo.pStages = &params.ShaderStages[0];
	pipelineInfo.stageCount = params.ShaderStages.Size();
}
