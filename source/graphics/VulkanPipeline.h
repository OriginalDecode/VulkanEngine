#pragma once
#include "graphics/vulkan/VulkanRasterizerStateInfos.h"
#include "graphics/vulkan/VulkanDepthStencilInfos.h"
#include "core/Types.h"
#include "core/containers/DynArray.h"
#include "core/Defines.h"

DEFINE_VK_HANDLE(VkPipeline);
DEFINE_VK_HANDLE(VkDescriptorPool);
DEFINE_VK_HANDLE(VkDescriptorSet);
DEFINE_VK_HANDLE(VkDescriptorSetLayout);
DEFINE_VK_HANDLE(VkPipelineLayout);

class VulkanPipeline
{
public:
	VulkanPipeline(const Core::DynArray<VkVertexInputBindingDescription>& vertex_binding_desc, const Core::DynArray<VkVertexInputAttributeDescription>& vertex_atr_desc,
				   const Core::DynArray<VkPipelineShaderStageCreateInfo>& shader_stages, const Core::DynArray<VkDescriptorSetLayoutBinding>& descriptor_bindings,
				   const Core::DynArray<VkPushConstantRange>& shader_constants, vlk::ERasterizerStateInfo raster_state, vlk::EDepthStencilInfo depth_state);

	void Set();

	void AddDescriptorSet(VkWriteDescriptorSet write_set);

private:
	VkPipeline m_Pipeline = nullptr;
	VkPipelineLayout m_PipelineLayout = nullptr;
	VkDescriptorPool m_DescriptorPool = nullptr;
	VkDescriptorSet m_DescriptorSet = nullptr; /* This gets described by the Descriptors ?*/
	VkDescriptorSetLayout m_DescriptorLayout = nullptr;
};
