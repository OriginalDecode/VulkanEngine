#pragma once

#include "GraphicsDevice.h"

#include "Core/utilities/utilities.h"
#include "Core/Defines.h"

#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

class Window;
//struct VkWin32SurfaceCreateInfoKHR;

namespace Graphics
{
	class ConstantBuffer;
	class VlkInstance;
	class VlkPhysicalDevice;
	class VlkDevice;
	class VlkSwapchain;

	class vkGraphicsDevice final : public IGraphicsDevice
	{
	public:
		vkGraphicsDevice();
		~vkGraphicsDevice();

		bool Init( const Window& window );

		void DrawFrame( float dt );

		VlkInstance& GetVlkInstance() { return *m_Instance; }
		VlkDevice& GetVlkDevice() { return *m_LogicalDevice; }

		virtual void BindConstantBuffer(ConstantBuffer* constantBuffer, uint32 offset) override;
		virtual void CreateConstantBuffer(ConstantBuffer* constantBuffer) override;
		virtual void DestroyConstantBuffer(ConstantBuffer* constantBuffer) override;

		void LoadShader(struct Shader* shader, const char* filepath);
		void DestroyShader(struct Shader* pShader);
	private:
		std::unique_ptr<VlkInstance> m_Instance;
		std::unique_ptr<VlkPhysicalDevice> m_PhysicalDevice;
		std::unique_ptr<VlkDevice> m_LogicalDevice;
		std::unique_ptr<VlkSwapchain> m_Swapchain;

		std::vector<VkCommandBuffer> m_CmdBuffers;
		VkCommandPool m_CmdPool = nullptr;

		VkSemaphore m_AcquireNextImageSemaphore = nullptr;

		VkSemaphore m_DrawDone = nullptr;
		VkFence m_CommandFence = nullptr;
		VkFence m_FrameFence[2]{ nullptr, nullptr };
		uint32 m_Index = 0;
		std::vector<VkFramebuffer> m_FrameBuffers;


		void SetupRenderCommands(int index);

		void SetupScissorArea();
		void SetupViewport();
		void CreateRenderPass();
		void CreateCommandPool();
		void CreateCommandBuffer();
		VkPipeline CreateGraphicsPipeline();

		VkDescriptorSetLayout CreateDescriptorLayout(VkDescriptorSetLayoutBinding* descriptorBindings, int32 bindingCount);
		void CreateDescriptorPool();
		void CreateDescriptorSet();

		VkPipelineLayout CreatePipelineLayout(VkDescriptorSetLayout* descriptorLayouts, int32 descriptorLayoutCount, VkPushConstantRange* pushConstantRange, int32 pushConstantRangeCount);
		VkImageView CreateImageView(VkFormat format, VkImage image);
		void CreateFramebuffers();

		VkVertexInputBindingDescription CreateBindDesc();
		VkVertexInputAttributeDescription CreateAttrDesc( int location, int offset );

		VkSemaphore CreateVkSemaphore( VkDevice pDevice );
		VkShaderModule LoadShader( const char* filepath, VkDevice pDevice );


	};

	class ConstantBuffer
	{
	public:
		struct Variable
		{
			Variable() = default;
			Variable( void* var, uint32 size )
				: var( var )
				, size( size )
			{
			}
			void* var = nullptr;
			uint32 size = 0;
		};

		void SetBuffer( void* buffer ) { m_Buffer = buffer; }
		void SetDeviceMemory( void* memory ) { m_DeviceMemory = memory; }

		template <typename T>
		void RegVar( T* var );

		void* GetBuffer() { return m_Buffer; }
		void* GetDeviceMemory() { return m_DeviceMemory; }
		uint32 GetSize() const { return m_BufferSize; }
		void* GetData() { return m_Vars.data(); }
		const std::vector<Variable>& GetVariables() const { return m_Vars; }

	private:
		uint32 m_BufferSize = 0;
		std::vector<Variable> m_Vars;
		void* m_Buffer = nullptr;
		void* m_DeviceMemory = nullptr;
	};

	template <typename T>
	void ConstantBuffer::RegVar( T* var )
	{
		m_Vars.push_back( { var, sizeof( T ) } );
		m_BufferSize += sizeof( T );
	}

}; // namespace Graphics