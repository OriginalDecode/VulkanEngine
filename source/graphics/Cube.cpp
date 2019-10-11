#include "Cube.h"
constexpr Core::Vector4f v0{ -1.f, -1.f, -1.f };
constexpr Core::Vector4f v1{ -1.f, 1.f, -1.f };
constexpr Core::Vector4f v2{ 1.f, 1.f, -1.f };
constexpr Core::Vector4f v3{ 1.f, -1.f, -1.f };

constexpr Core::Vector4f v4{ -1.f, -1.f, 1.f };
constexpr Core::Vector4f v5{ -1.f, 1.f, 1.f };
constexpr Core::Vector4f v6{ 1.f, 1.f, 1.f };
constexpr Core::Vector4f v7{ 1.f, -1.f, 1.f };

// position & normal
constexpr Vertex _cube[] = {
	// front (facing camera)
	{ v2, { 0.f, 0.f, -1.f } }, // 0
	{ v0, { 0.f, 0.f, -1.f } }, // 1
	{ v1, { 0.f, 0.f, -1.f } }, // 2

	{ v2, { 0.f, 0.f, -1.f } }, // 3
	{ v3, { 0.f, 0.f, -1.f } }, // 4
	{ v0, { 0.f, 0.f, -1.f } }, // 5

	// right
	{ v6, { 1.f, 0.f, 0.f } }, // 6
	{ v3, { 1.f, 0.f, 0.f } }, // 7
	{ v2, { 1.f, 0.f, 0.f } }, // 8

	{ v6, { 1.f, 0.f, 0.f } }, // 9
	{ v7, { 1.f, 0.f, 0.f } }, // 10
	{ v3, { 1.f, 0.f, 0.f } }, // 11

	// back (away from camera)
	{ v5, { 0.f, 0.f, 1.f } }, // 12
	{ v7, { 0.f, 0.f, 1.f } }, // 13
	{ v6, { 0.f, 0.f, 1.f } }, // 14

	{ v5, { 0.f, 0.f, 1.f } }, // 15
	{ v4, { 0.f, 0.f, 1.f } }, // 16
	{ v7, { 0.f, 0.f, 1.f } }, // 17

	// left
	{ v1, { -1.f, 0.f, 0.f } }, // 18
	{ v4, { -1.f, 0.f, 0.f } }, // 19
	{ v5, { -1.f, 0.f, 0.f } }, // 20

	{ v1, { -1.f, 0.f, 0.f } }, // 21
	{ v0, { -1.f, 0.f, 0.f } }, // 22
	{ v4, { -1.f, 0.f, 0.f } }, // 23

	// top
	{ v6, { 0.f, 1.f, 0.f } }, // 24
	{ v1, { 0.f, 1.f, 0.f } }, // 25
	{ v5, { 0.f, 1.f, 0.f } }, // 26

	{ v6, { 0.f, 1.f, 0.f } }, // 27
	{ v2, { 0.f, 1.f, 0.f } }, // 28
	{ v1, { 0.f, 1.f, 0.f } }, // 29

	// bottom
	{ v3, { 0.f, -1.f, 0.f } }, // 30
	{ v4, { 0.f, -1.f, 0.f } }, // 31
	{ v0, { 0.f, -1.f, 0.f } }, // 32

	{ v3, { 0.f, -1.f, 0.f } }, // 33
	{ v7, { 0.f, -1.f, 0.f } }, // 34
	{ v4, { 0.f, -1.f, 0.f } }, // 35
};

namespace Graphics
{

	Cube::~Cube()
	{
		// vkDestroyBuffer( device, m_VertexBuffer.m_VertexBuffer.m_Buffer, nullptr );
	}

	void Cube::Init(VkDevice device, VkPhysicalDevice physicalDevice)
	{
		m_Orientation = Core::Matrix44f::Identity();
		m_VertexBuffer.m_Stride = sizeof( Vertex );
		m_VertexBuffer.m_VertexCount = ARRSIZE( _cube );
		m_VertexBuffer.m_Offset = 0;

		const int32 dataSize = m_VertexBuffer.m_Stride * m_VertexBuffer.m_VertexCount;

		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = dataSize;
		createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkBuffer buffer;
		VkDeviceMemory deviceMem;

		// CreateBuffer( createInfo, &buffer, &deviceMem, device, physicalDevice );

		m_VertexBuffer.m_VertexBuffer.m_Buffer = buffer;


		void* data = nullptr;
		if( vkMapMemory( device, deviceMem, 0, dataSize, 0, &data ) != VK_SUCCESS )
			ASSERT( false, "Failed to map memory!" );
		memcpy( data, _cube, dataSize );
		vkUnmapMemory( device, deviceMem );
	}

	void Cube::Draw() {}

}; // namespace Graphics