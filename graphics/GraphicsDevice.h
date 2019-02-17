#pragma once
#include "Core/Types.h"
#include "Utilities.h"


/*
	The usage of the graphics device class is to handle creation of buffers, shaders and various
	other resources that the graphics cards needs to operate.

	The class IGraphicsDevice is to ensure a common interface for different graphics devices.
	Such as vulkan and d3d11/d3d12.
	Vulkan and D3D12 does share more similarities, but code can still be put behind a common interface
	with D3D11. 

	We will start with building the vulkan graphics backend and then move on to the D3D11 backend, 
	and finish with the D3D12. Other backends should be able to be added through this interface as well.

	It should be clear and documented of what each function does and should not leave the programmer
	wondering of what a function is supposed to do since that would lead to confusion and that is
	bad for development time.

	If something gets too complex too fast, it's better to rething and redesign at an early stage than
	to proceed with something that will not work in the long run.

	Sometimes complexity is needed for stability and performance, but one should not have to sacriface 
	one for the other.

	It should be clear and well defined what something does and the same code should not be written twice (preferably )

*/
namespace Graphics
{
	class IGraphicsDevice
	{
		public:
		protected:
		private:
	};

}; //namespace Graphics



// #ifdef _WIN32
// struct ID3D11RenderTargetView;
// struct ID3D11DepthStencilView;
// struct ID3D11DeviceContext;
// struct ID3D11Buffer;
// struct D3D11_INPUT_ELEMENT_DESC;
// #endif

// class Window;

// namespace Graphics
// {
// 	struct BufferDesc
// 	{
// 		BufferDesc() = default;
// 		BufferDesc(EBindFlag bindFlag, ECPUAccessFlag cpuAccessFlag, EUsage usageFlag, uint8* data, int32 size, int32 stride, int32 miscFlags, int32 byteWidth) 
// 			: m_BindFlag(bindFlag)
// 			, m_CPUAccessFlag(cpuAccessFlag)
// 			, m_UsageFlag(usageFlag)
// 			, m_Data(data)
// 			, m_Size(size)
// 			, m_StructuredByteStride(stride)
// 			, m_MiscFlags(miscFlags)
// 			, m_ByteWidth(byteWidth)
// 		{
// 		}

// 		EBindFlag m_BindFlag = EBindFlag::NONE;
// 		ECPUAccessFlag m_CPUAccessFlag = ECPUAccessFlag::NO_ACCESS_FLAG;
// 		EUsage m_UsageFlag = EUsage::DEFAULT_USAGE;
// 		uint8* m_Data = nullptr;
// 		int32 m_Size = 0;
// 		int32 m_StructuredByteStride = 0;
// 		int32 m_MiscFlags = 0;
// 		int32 m_ByteWidth = 0;
// 	};

// #ifdef _WIN32
// 	ID3D11DeviceContext* GetContext();
// 	long Present(uint32 syncInterval, uint32 flags);
// #endif

// 	class GraphicsDevice
// 	{
// 	public:
// 		GraphicsDevice();
// 		~GraphicsDevice();

// #ifdef _WIN32
// 		void CreateDevice(const Window& window, ID3D11RenderTargetView** defaultRenderTarget, ID3D11DepthStencilView** defaultDepthStencil);
// 		ID3D11Buffer* CreateBuffer(const BufferDesc& buffer_desc);
// 		ID3D11InputLayout* CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* desc, uint32 elementCount, ID3D10Blob* shaderBlob);
// 		void* CreateShader(ID3D10Blob* blobData, ShaderType shaderType );
// #endif

// 		void SetDebugName(void* pResource, const char* objectName);
// 	private:

// 	};

// }; //namespace Graphics