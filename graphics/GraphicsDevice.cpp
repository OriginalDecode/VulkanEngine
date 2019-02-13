#include "GraphicsDevice.h"

#include "Window.h"

#include <d3d11.h>
#include <cassert>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxguid.lib" ) 
#pragma comment( lib, "D3DCompiler.lib" )
#pragma comment( lib, "dxgi.lib" )
//d3d11.lib

ID3D11Device* device = nullptr;
IDXGISwapChain* swapchain = nullptr;

namespace Graphics
{

	ID3D11DeviceContext* GetContext()
	{
		ID3D11DeviceContext* context = nullptr;
		device->GetImmediateContext(&context);
		return context;
	}

	HRESULT Present(uint32 syncInterval, uint32 flags)
	{
		return swapchain->Present(syncInterval, flags);
	}

	GraphicsDevice::GraphicsDevice()
	{
	}


	GraphicsDevice::~GraphicsDevice()
	{
		swapchain->Release();
		device->Release();
	}

	void GraphicsDevice::CreateDevice(const Window& window, ID3D11RenderTargetView** defaultRenderTarget, ID3D11DepthStencilView** defaultDepthStencil)
	{
		DXGI_SWAP_CHAIN_DESC scDesc;
		ZeroMemory(&scDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		const Window::Size& window_size = window.GetInnerSize();

		scDesc.BufferCount = 1;
		scDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		scDesc.BufferDesc.Width = UINT(window_size.m_Width);
		scDesc.BufferDesc.Height = UINT(window_size.m_Height);
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.OutputWindow = window.GetHandle();
		scDesc.SampleDesc.Count = 1;
		scDesc.SampleDesc.Quality = 0;
		scDesc.Windowed = true;

		scDesc.Flags = 0;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		const D3D_FEATURE_LEVEL requested_feature_levels[] = {
			D3D_FEATURE_LEVEL_11_0,
		};
		const UINT feature_count = ARRAYSIZE(requested_feature_levels);

		UINT create_device_flags = 0;
#ifdef _DEBUG
		create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr,
												   D3D_DRIVER_TYPE_HARDWARE,
												   nullptr,
												   create_device_flags,
												   requested_feature_levels,
												   feature_count,
												   D3D11_SDK_VERSION,
												   &scDesc,
												   &swapchain,
												   &device,
												   nullptr,
												   nullptr);

		assert(!FAILED(hr) && "Failed to create Device and Swapchain");


		ID3D11DeviceContext* context = nullptr;
		device->GetImmediateContext(&context);

		ID3D11Texture2D* backbuffer = nullptr;

		hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);
		assert(!FAILED(hr) && "Failed to create backbuffer");

		hr = swapchain->SetFullscreenState(FALSE, nullptr);
		assert(!FAILED(hr) && "Failed to set to fullscreen");

		hr = device->CreateRenderTargetView(backbuffer, nullptr, defaultRenderTarget);
		assert(!FAILED(hr) && "Failed to create RenderTargetView");
		backbuffer->Release();

		D3D11_TEXTURE2D_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));

		depthDesc.Width = UINT(window_size.m_Width);
		depthDesc.Height = UINT(window_size.m_Height);
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D32_FLOAT; //DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = 1; //sample
		depthDesc.SampleDesc.Quality = 0; //quality pattern
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ID3D11Texture2D* depth_buffer = nullptr;
		hr = device->CreateTexture2D(&depthDesc, nullptr, &depth_buffer);
		assert(!FAILED(hr) && "Failed to create texture for depthbuffer");

		D3D11_DEPTH_STENCIL_VIEW_DESC stencilDesc;
		ZeroMemory(&stencilDesc, sizeof(stencilDesc));
		stencilDesc.Format = depthDesc.Format;
		stencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		stencilDesc.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(depth_buffer, &stencilDesc, defaultDepthStencil);
		assert(!FAILED(hr) && "Failed to create depthstencil view");
		depth_buffer->Release();

		context->Release();
	}



	ID3D11Buffer* GraphicsDevice::CreateBuffer(const BufferDesc& buffer_desc)
	{
		ID3D11Buffer* buffer = nullptr;
		D3D11_BUFFER_DESC desc;
		desc.BindFlags = Graphics::GetBindFlag(buffer_desc.m_BindFlag);
		desc.CPUAccessFlags = Graphics::GetCPUAccessFlag(buffer_desc.m_CPUAccessFlag);
		desc.Usage = Graphics::GetUsage(buffer_desc.m_UsageFlag);
		desc.MiscFlags = buffer_desc.m_MiscFlags;
		desc.StructureByteStride = buffer_desc.m_StructuredByteStride;
		desc.ByteWidth = buffer_desc.m_ByteWidth;


		D3D11_SUBRESOURCE_DATA srd;
		ZeroMemory(&srd, sizeof(D3D11_SUBRESOURCE_DATA));

		if (buffer_desc.m_Data)
		{
			srd.pSysMem = buffer_desc.m_Data;
			HRESULT hr = device->CreateBuffer(&desc, &srd, &buffer);
			assert(!FAILED(hr) && "Failed to create buffer");

		}
		else
		{
			HRESULT hr = device->CreateBuffer(&desc, nullptr, &buffer);
			assert(!FAILED(hr) && "Failed to create buffer");

		}

		return buffer;
	}

	ID3D11InputLayout* GraphicsDevice::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* desc, uint32 elementCount, ID3D10Blob* shaderBlob)
	{
		assert(!"Missing vertex shader");
		ID3D11InputLayout* layout = nullptr;
		HRESULT hr = device->CreateInputLayout(desc, elementCount, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &layout);
		assert(!FAILED(hr) && "Failed to create input layout");
		return layout;
	}

	void* GraphicsDevice::CreateShader(ID3D10Blob* blobData, ShaderType shaderType)
	{
		
		switch (shaderType)
		{
			case EShaderType_VERTEX: 
			{	
				ID3D11VertexShader* shader = nullptr;
				device->CreateVertexShader(blobData->GetBufferPointer(), blobData->GetBufferSize(), nullptr, &shader);
				return shader;
			} break;
			case EShaderType_FRAGMENT:
			{
				ID3D11PixelShader* shader = nullptr;
				device->CreatePixelShader(blobData->GetBufferPointer(), blobData->GetBufferSize(), nullptr, &shader);
				return shader;
			} break;

			case EShaderType_GEOMETRY:
			{
				ID3D11GeometryShader* shader = nullptr;
				device->CreateGeometryShader(blobData->GetBufferPointer(), blobData->GetBufferSize(), nullptr, &shader);
				return shader;
			} break;
			case EShaderType_HULL:
			{
				ID3D11HullShader* shader = nullptr;
				device->CreateHullShader(blobData->GetBufferPointer(), blobData->GetBufferSize(), nullptr, &shader);
				return shader;
			} break;
			case EShaderType_DOMAIN:
			{
				ID3D11DomainShader* shader = nullptr;
				device->CreateDomainShader(blobData->GetBufferPointer(), blobData->GetBufferSize(), nullptr, &shader);
				return shader;
			} break;
			case EShaderType_COMPUTE:
			{
				ID3D11ComputeShader* shader = nullptr;
				device->CreateComputeShader(blobData->GetBufferPointer(), blobData->GetBufferSize(), nullptr, &shader);
				return shader;
			} break;
		}

		return nullptr;
	}

	void GraphicsDevice::SetDebugName(void* pResource, const char* objectName)
	{
		ID3D11DeviceChild* resource = static_cast<ID3D11DeviceChild*>(pResource);
		resource->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(objectName)), objectName);
	}

}; //namespace Graphics