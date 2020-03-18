#pragma once
#include "core/Types.h"
#include "core/containers/GrowingArray.h"
#include <vulkan/vulkan_core.h>

struct VulkanGraphicsPipelineParams
{

	uint32 Subpass;
	uint32 ColorWriteMask;
	uint32 ColorAttachmentCount;
	VkPrimitiveTopology Topology;
	VkCullModeFlagBits CullMode;
	VkRenderPass RenderPass;

	bool BlendEnable : 2;
	bool DepthTestEnable : 2;
	bool StencilTestEnable : 2;
	bool DepthWriteEnable : 2;
	struct
	{
		VkBlendFactor SrcColor;
		VkBlendFactor DstColor;
		VkBlendFactor SrcAlpha;
		VkBlendFactor DstAlpha;
	} BlendFactor;

	struct
	{
		uint32 RefMask;
		uint32 WriteMask;
		uint32 CompMask;
		VkCompareOp CompareOp;
		VkStencilOp DepthFailOp;
		VkStencilOp PassOp;
		VkStencilOp FailOp;
	} Stencil;

	struct
	{
		VkCompareOp CompareOp;
	} Depth;

	Core::GrowingArray<VkDescriptorSetLayout> Layouts;
	Core::GrowingArray<VkVertexInputBindingDescription> Bindings;
	Core::GrowingArray<VkVertexInputAttributeDescription> Attributes;
	Core::GrowingArray<VkPushConstantRange> PushConstantRanges;
	Core::GrowingArray<VkPipelineShaderStageCreateInfo> ShaderStages;
};

class VulkanGraphicsPipeline
{

public:
	VulkanGraphicsPipeline() = default;
	~VulkanGraphicsPipeline() = default;

	void Init(const VulkanGraphicsPipelineParams& pipeline_params);
	void Release();

	const VkPipeline Get() const { return m_Pipeline; }

private:
	VkPipelineLayout m_Layout = nullptr;
	VkPipeline m_Pipeline = nullptr;

	void SetupViewportStateInfo(VkPipelineViewportStateCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params);
	void SetupMultiSamplerInfo(VkPipelineMultisampleStateCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params);
	void SetupBlendStateInfo(VkPipelineColorBlendStateCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params);
	void SetupInputAssemblerInfo(VkPipelineInputAssemblyStateCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params);
	void SetupVertexInputStateInfo(VkPipelineVertexInputStateCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params);
	void SetupRasetrizerInfo(VkPipelineRasterizationStateCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params);
	void SetupDepthStencilInfo(VkPipelineDepthStencilStateCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params);

	void SetupPipelineLayout(VkPipelineLayoutCreateInfo* info, const VulkanGraphicsPipelineParams& pipeline_params);

	void SetupGraphicsPipelineInfo(VkGraphicsPipelineCreateInfo* info, const VkPipelineLayoutCreateInfo& layout_info, const VkPipelineRasterizationStateCreateInfo& raster_info,
								   const VkPipelineVertexInputStateCreateInfo& vtx_input_info, const VkPipelineInputAssemblyStateCreateInfo& ia_info,
								   const VkPipelineColorBlendStateCreateInfo& blend_info, const VkPipelineMultisampleStateCreateInfo& multi_sample_info,
								   const VkPipelineViewportStateCreateInfo& vp_info, const VkPipelineDepthStencilStateCreateInfo& depth_state_info,
								   const VulkanGraphicsPipelineParams& params);
};
