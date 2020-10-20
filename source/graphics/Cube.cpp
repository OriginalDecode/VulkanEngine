#include "Cube.h"

#include <vulkan/vulkan_core.h>

#include "VlkDevice.h"
#include "VlkPhysicalDevice.h"

#include "core/File.h"
#include "logger/Debug.h"

void Cube::Init(Graphics::VlkDevice* device, Graphics::VlkPhysicalDevice* physicalDevice)
{
	Core::File loader("cube.mdl", Core::File::READ_FILE);

	m_Orientation = Core::Matrix44f::Identity();
	m_Stride = sizeof(Vertex);
	m_VertexCount = (loader.GetSize() / sizeof(Vertex));
	m_Offset = 0;

	const int32 dataSize = m_Stride * m_VertexCount;

	VkBufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = dataSize;
	createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	m_Buffer = device->CreateBuffer(createInfo, &m_Memory, physicalDevice);

	void* data = nullptr;
	if(vkMapMemory(device->GetDevice(), m_Memory, 0, dataSize, 0, &data) != VK_SUCCESS)
		ASSERT(false, "Failed to map memory!");

	memcpy(data, loader.GetBuffer(), loader.GetSize());
	vkUnmapMemory(device->GetDevice(), m_Memory);
}

void Cube::Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
	char* data = new char[sizeof(Core::Matrix44f)];
	memset(data, 0, sizeof(Core::Matrix44f));
	memcpy(&data[0], &m_Orientation, sizeof(Core::Matrix44f));
	// memcpy(&data[sizeof(Core::Matrix44f)], &_LightDir, sizeof(Core::Vector4f)); // This is not good at all

	vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Core::Matrix44f), data);
	/* This is quite a strange one, this is only gonna be available in non-instanced entities for position */

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_Buffer, &offset);

	// This executes secondary commandBuffers inside of the primary commandBuffer
	// vkCmdExecuteCommands(commandBuffer, 0, nullptr);

	vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);

	delete[] data;
	data = nullptr;
}

void Cube::SetPosition(const Core::Vector4f& position)
{
	m_Orientation.SetPosition(position);
}

void Cube::Destroy(VkDevice device)
{
	vkDestroyBuffer(device, m_Buffer, nullptr);
}
