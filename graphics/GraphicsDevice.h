#pragma once
#include "Core/Types.h"
#include "Utilities.h"

#ifdef _WIN32
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct D3D11_INPUT_ELEMENT_DESC;
#endif

class Window;

namespace Graphics
{
	struct BufferDesc
	{
		BufferDesc() = default;
		BufferDesc(EBindFlag bindFlag, ECPUAccessFlag cpuAccessFlag, EUsage usageFlag, uint8* data, int32 size, int32 stride, int32 miscFlags, int32 byteWidth) 
			: m_BindFlag(bindFlag)
			, m_CPUAccessFlag(cpuAccessFlag)
			, m_UsageFlag(usageFlag)
			, m_Data(data)
			, m_Size(size)
			, m_StructuredByteStride(stride)
			, m_MiscFlags(miscFlags)
			, m_ByteWidth(byteWidth)
		{
		}

		EBindFlag m_BindFlag = EBindFlag::NONE;
		ECPUAccessFlag m_CPUAccessFlag = ECPUAccessFlag::NO_ACCESS_FLAG;
		EUsage m_UsageFlag = EUsage::DEFAULT_USAGE;
		uint8* m_Data = nullptr;
		int32 m_Size = 0;
		int32 m_StructuredByteStride = 0;
		int32 m_MiscFlags = 0;
		int32 m_ByteWidth = 0;
	};

#ifdef _WIN32
	ID3D11DeviceContext* GetContext();
	long Present(uint32 syncInterval, uint32 flags);
#endif

	class GraphicsDevice
	{
	public:
		GraphicsDevice();
		~GraphicsDevice();

#ifdef _WIN32
		void CreateDevice(const Window& window, ID3D11RenderTargetView** defaultRenderTarget, ID3D11DepthStencilView** defaultDepthStencil);
		ID3D11Buffer* CreateBuffer(const BufferDesc& buffer_desc);
		ID3D11InputLayout* CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* desc, uint32 elementCount, ID3D10Blob* shaderBlob);
		void* CreateShader(ID3D10Blob* blobData, ShaderType shaderType );
#endif

		void SetDebugName(void* pResource, const char* objectName);
	private:

	};

}; //namespace Graphics