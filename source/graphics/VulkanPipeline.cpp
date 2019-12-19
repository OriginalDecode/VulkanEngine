#include "VulkanPipeline.h"
#include "graphics/vulkan/VulkanUtils.h"

VulkanPipeline::VulkanPipeline(const Core::DynArray<VkVertexInputBindingDescription>& vertex_binding_desc, const Core::DynArray<VkVertexInputAttributeDescription>& vertex_atr_desc,
							   const Core::DynArray<VkPipelineShaderStageCreateInfo>& shader_stages, const Core::DynArray<VkDescriptorSetLayoutBinding>& descriptor_bindings,
							   const Core::DynArray<VkPushConstantRange>& shader_constants, vlk::ERasterizerStateInfo raster_state, vlk::EDepthStencilInfo depth_state)
{

	m_DescriptorLayout = vlk::CreateDescLayout(&descriptor_bindings[0], descriptor_bindings.Size());

	m_PipelineLayout = vlk::CreatePipelineLayout(&m_DescriptorLayout, 1, &shader_constants[0], shader_constants.Size());

	VkPipelineVertexInputStateCreateInfo input_info = {};
	input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	input_info.vertexBindingDescriptionCount = vertex_binding_desc.Size();
	input_info.pVertexBindingDescriptions = &vertex_binding_desc[0];

	input_info.vertexAttributeDescriptionCount = vertex_atr_desc.Size();
	input_info.pVertexAttributeDescriptions = &vertex_atr_desc[0];
}
