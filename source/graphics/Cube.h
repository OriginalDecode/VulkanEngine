#pragma once
#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Math/Matrix44.h"

namespace Graphics
{

	class VlkDevice;
	class VlkPhysicalDevice;
}; // namespace Graphics

DEFINE_HANDLE(VkBuffer);
DEFINE_HANDLE(VkDeviceMemory);
DEFINE_HANDLE(VkDevice);
DEFINE_HANDLE(VkCommandBuffer);
DEFINE_HANDLE(VkPipelineLayout);

// Vertex Description
struct Vertex
{
	Core::Vector4f position;
	Core::Vector4f color;
	Core::Vector4f normal;
};

class Cube
{
public:
	Cube() = default;
	~Cube() = default;

	void Init(Graphics::VlkDevice* device, Graphics::VlkPhysicalDevice* physicalDevice);
	void Destroy(VkDevice device);
	void Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
	void SetPosition(const Core::Vector4f& position);

private:
	Core::Matrix44f m_Orientation;

	VkBuffer m_Buffer = nullptr;
	VkDeviceMemory m_Memory = nullptr;
	int32 m_VertexCount = 0;
	int32 m_Stride = 0;
	int32 m_Offset = 0;
};
